@echo off
git add .
for /f "tokens=1,2 delims==" %%I in ('wmic OS Get localdatetime /value') do (
    if "%%I"=="LocalDateTime" set datetime=%%J
)
set day=%datetime:~6,2%
set month=%datetime:~4,2%
git commit -m "Update %day%.%month%"
git push