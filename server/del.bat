@echo off
for /r %%i in (.vs\server\v15\debug,.vs\server\v15\release,.vs\server\v15\ipch) do rd /s /q "%%i"
for /r %%i in (.vs\server\v15\*.sdf,.vs\server\v15\*.VC.db) do del /s /f /q "%%i"
pause