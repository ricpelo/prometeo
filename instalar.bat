@echo off

rem
rem Programa de instalaci¢n de PROMETEO - Generador de Aventuras Conversacionales
rem (c) Ricardo P‚rez L¢pez - Alpha Aventuras
rem

echo.
echo Utilidad de instalaci¢n de PROMETEO
echo.
if "%1" == "" goto formato
echo Instalando PROMETEO en %1. Por favor, espere...
md %1 > nul
copy *.* %1 > nul
echo.
echo Instalaci¢n finalizada...
echo.
goto end

:formato
echo Formato: "INSTALAR <x:\directorio_destino>" 
echo.         Hay que indicar tambi‚n la unidad de destino
echo.
echo Ejemplo: "INSTALAR c:\util\gac"
echo.

:end
