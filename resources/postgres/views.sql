\c gasmeter

-- create daily view with data from archive and cagg_daily
DROP MATERIALIZED VIEW IF EXISTS daily_view CASCADE;

CREATE MATERIALIZED VIEW daily_view
AS
SELECT
  bucket_1d AS time,
  volume_1d AS volume,
  volume_1d * factor AS energy, 
  volume_1d * factor * price + rate / 365.0 AS bill,
  total
FROM archive JOIN plan ON archive.plan_id = plan.id
GROUP BY bucket_1d, volume_1d, total, bill, factor
UNION
SELECT
  bucket_1d AS time,
  volume_1d AS volume,
  volume_1d * factor AS energy,
  volume_1d * factor * price + rate / 365.0 AS bill,
  total
FROM cagg_daily JOIN plan ON cagg_daily.plan_id = plan.id
--- insert end time of archive
WHERE bucket_1d > TIMESTAMP WITH TIME ZONE '2022-02-02 01:00:00+01'
GROUP BY bucket_1d, volume_1d, total, bill, factor
ORDER BY time;

-- index
CREATE UNIQUE INDEX daily_idx ON daily_view (time);

-- grant
GRANT SELECT ON TABLE daily_view TO grafana;

--
-- create monthly view
--
CREATE MATERIALIZED VIEW monthly_view
AS
SELECT
  timescaledb_experimental.time_bucket_ng('1 month', time) AS time,
  sum(volume) AS volume,
  sum(energy) AS energy,
  sum(bill) AS bill,
  first(total, time) AS total,
  avg(volume) AS avg,
  min(volume) AS min,
  max(volume) AS max
FROM daily_view
GROUP BY timescaledb_experimental.time_bucket_ng('1 month', time)
ORDER BY time;

-- index
CREATE UNIQUE INDEX monthly_idx ON monthly_view (time);

-- grant
GRANT SELECT ON TABLE monthly_view TO grafana;

--
-- create yearly view
--
CREATE MATERIALIZED VIEW yearly_view
AS
SELECT
  timescaledb_experimental.time_bucket_ng('1 year', time) AS time,
  sum(volume) AS volume,
  sum(energy) AS energy,
  sum(bill) AS bill,
  first(total, time) AS total
FROM daily_view
GROUP BY timescaledb_experimental.time_bucket_ng('1 year', time)
ORDER BY time;

-- index
CREATE UNIQUE INDEX yearly_idx ON yearly_view (time);

-- grant
GRANT SELECT ON TABLE yearly_view TO grafana;
