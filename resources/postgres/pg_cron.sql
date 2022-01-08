\c pg_cron

CREATE EXTENSION IF NOT EXISTS pg_cron;

-- regularly purge the log
SELECT cron.schedule('purge_log', '13 0 * * *', $$DELETE
    FROM cron.job_run_details
    WHERE end_time < now() - INTERVAL '3 days'$$);

-- schedule updates
SELECT cron.schedule('gasmeter_daily', '1,16,31,46 * * * *', $$REFRESH
  MATERIALIZED VIEW CONCURRENTLY daily_view$$);

SELECT cron.schedule('gasmeter_monthly', '1,16,31,46 * * * *', $$REFRESH
  MATERIALIZED VIEW CONCURRENTLY monthly_view$$);

SELECT cron.schedule('gasmeter_yearly', '1,16,31,46 * * * *', $$REFRESH
  MATERIALIZED VIEW CONCURRENTLY yearly_view$$);

UPDATE cron.job SET database = 'gasmeter' WHERE 
  jobname = 'gasmeter_daily' OR 
  jobname = 'gasmeter_monthly' OR
  jobname = 'gasmeter_yearly';
