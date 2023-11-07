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
WHERE bucket_1d > TIMESTAMP WITH TIME ZONE '2023-08-16 00:00:00+02'
GROUP BY bucket_1d, volume_1d, total, bill, factor
ORDER BY time;

-- index
CREATE UNIQUE INDEX daily_idx ON daily_view (time);

-- grant
GRANT SELECT ON TABLE daily_view TO grafana;
GRANT SELECT ON TABLE daily_view TO nodejs;

--
-- create monthly view
--
CREATE MATERIALIZED VIEW monthly_view
AS
SELECT
  time_bucket('1 month', time, 'Europe/Berlin') AS bucket_1m,
  sum(volume) AS volume,
  sum(energy) AS energy,
  sum(bill) AS bill,
  first(total, time) AS total,
  avg(volume) AS avg,
  min(volume) AS min,
  max(volume) AS max
FROM daily_view
GROUP BY bucket_1m
ORDER BY bucket_1m;

-- index
CREATE UNIQUE INDEX monthly_idx ON monthly_view (bucket_1m);

-- grant
GRANT SELECT ON TABLE monthly_view TO grafana;
GRANT SELECT ON TABLE monthly_view TO nodejs;

--
-- create yearly view
--
CREATE MATERIALIZED VIEW yearly_view
AS
SELECT
  time_bucket('1 year', time, 'Europe/Berlin') AS bucket_1y,
  count(*) as days,
  sum(volume) AS volume,
  sum(energy) AS energy,
  sum(bill) AS bill,
  first(total, time) AS total
FROM daily_view
GROUP BY bucket_1y
ORDER BY bucket_1y;

-- index
CREATE UNIQUE INDEX yearly_idx ON yearly_view (bucket_1y);

-- grant
GRANT SELECT ON TABLE yearly_view TO grafana;
GRANT SELECT ON TABLE yearly_view TO nodejs;
