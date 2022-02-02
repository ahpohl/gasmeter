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
GROUP BY bucket_1d, volume_1d, total, bill, factor
ORDER BY time;

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
  first(total, time) AS total
FROM daily_view
GROUP BY timescaledb_experimental.time_bucket_ng('1 month', time)
ORDER BY time;

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

-- grant
GRANT SELECT ON TABLE yearly_view TO grafana;
