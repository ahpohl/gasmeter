\c gasmeter

DROP MATERIALIZED VIEW IF EXISTS "monthly_view" CASCADE;
DROP MATERIALIZED VIEW IF EXISTS "daily_view" CASCADE;
DROP MATERIALIZED VIEW IF EXISTS "cagg_daily" CASCADE;
DROP TABLE IF EXISTS "archive" CASCADE;
DROP TABLE IF EXISTS "live" CASCADE;
DROP TABLE IF EXISTS "sensors" CASCADE;
DROP TABLE IF EXISTS "plan" CASCADE;

CREATE EXTENSION IF NOT EXISTS timescaledb;

CREATE TABLE "sensors" (
  id SERIAL PRIMARY KEY,
  serial_num VARCHAR(50),
  customer_id VARCHAR(50),
  part_num VARCHAR(50),
  mfg_date VARCHAR(50)
);

CREATE TABLE "plan" (
  id SERIAL PRIMARY KEY,
  price DOUBLE PRECISION,
  rate DOUBLE PRECISION,
  factor DOUBLE PRECISION
);

CREATE TABLE "live" (
  time TIMESTAMPTZ NOT NULL,
  sensor_id INTEGER NOT NULL,
  plan_id INTEGER NOT NULL,
  volume DOUBLE PRECISION,
  state INTEGER,
  CONSTRAINT sensor_id FOREIGN KEY (sensor_id) REFERENCES sensors (id),
  CONSTRAINT plan_id FOREIGN KEY (plan_id) REFERENCES plan (id)
);

SELECT create_hypertable('live', 'time');
SELECT add_retention_policy('live', INTERVAL '14 days');

INSERT INTO sensors(id, serial_num, customer_id, part_num, mfg_date) VALUES
(1, '42010646', '20141182555', 'G4RF1c', '2020');

INSERT INTO plan(id, price, rate, factor) VALUES
(1, 0.0583, 261.80, 10.835);

GRANT INSERT, SELECT ON TABLE live TO nodejs;
GRANT SELECT ON TABLE live TO grafana;

GRANT SELECT ON TABLE sensors TO nodejs;
GRANT SELECT ON TABLE sensors TO grafana;

GRANT SELECT ON TABLE plan TO nodejs;
GRANT SELECT ON TABLE plan TO grafana;
