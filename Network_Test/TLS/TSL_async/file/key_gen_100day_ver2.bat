@echo off
call :initColorPrint
title ######### Ű ���� #########
echo.
echo.

echo ���� ���丮�� KEY�� �����Ѵ�
cd
call :colorPrint FC "�� KEY�� Common Name�� �ʼ� �Է�"
echo.
call :colorPrint FC "�� KEY�� Common Name�� �ʼ� �Է�"
echo.


echo.
echo.
pause

cls

openssl genrsa -out rootCA.key
cls
echo ############# rootCA������ ����
call :colorPrint FC "############# Common Name �ʼ� �Է�"
echo.
call :colorPrint FC "############# Common Name �ʼ� �Է�"
echo.
call :colorPrint FC "############# Common Name �ʼ� �Է�"
echo.
call :colorPrint FC "############# Common Name �ʼ� �Է�"
echo.
echo.
openssl req -x509 -new -nodes -key rootCA.key -days 100 -out rootCA.crt

cls
openssl genrsa -out server.key 2048
cls

echo ############# Server ���� ��û�� ����
call :colorPrint FC "############# Common Name �ʼ� �Է�(user�� �ٸ� �� �ʼ�)"
echo.
call :colorPrint FC "############# Common Name �ʼ� �Է�(user�� �ٸ� �� �ʼ�)"
echo.
call :colorPrint FC "############# Common Name �ʼ� �Է�(user�� �ٸ� �� �ʼ�)"
echo.
call :colorPrint FC "############# Common Name �ʼ� �Է�(user�� �ٸ� �� �ʼ�)"
echo.
echo.
openssl req -new -key server.key -out server.csr
cls

openssl x509 -req -in server.csr -CA rootCA.crt -CAkey rootCA.key -CAcreateserial -out server.crt -days 100

openssl verify -CAfile rootCA.crt server.crt
openssl verify -CAfile rootCA.crt rootCA.crt
echo.

call :colorPrint FC "############# ������ ���� ##############"
echo.
call :colorPrint FC "############# ������ ���� ##############"
echo.
call :colorPrint FC "############# ������ ���� ##############"
echo.
call :colorPrint FC "############# ������ ���� ##############"
echo.
openssl verify -CAfile server.crt server.crt
call :colorPrint FC "############# OK �߸� �ٽ� ���弼�� ##############"
echo.
call :colorPrint FC "############# OK �߸� �ٽ� ���弼�� ##############"
echo.
call :colorPrint FC "############# OK �߸� �ٽ� ���弼�� ##############"
echo.
call :colorPrint FC "############# OK �߸� �ٽ� ���弼�� ##############"
echo.

pause

openssl dhparam -out dh2048.pem 2048

openssl x509 -in server.crt -out server.pem -outform PEM
openssl x509 -in rootCA.crt -out rootCA.pem -outform PEM

del rootCA.crt
del server.crt
cls
set toyear=%date:~-10,4%
set tomons=%date:~-5,2%
set today=%date:~-2,2%
echo.
echo %toyear%/%tomons%/%today% KEY ���� �Ϸ� by KTY
echo.
pause
call :cleanupColorPrint
exit /b


:colorPrint Color  Str  [/n]
setlocal
set "str=%~2"
call :colorPrintVar %1 str %3
exit /b

:colorPrintVar  Color  StrVar  [/n]
if not defined %~2 exit /b
setlocal enableDelayedExpansion
set "str=a%DEL%!%~2:\=a%DEL%\..\%DEL%%DEL%%DEL%!"
set "str=!str:/=a%DEL%/..\%DEL%%DEL%%DEL%!"
set "str=!str:"=\"!"
pushd "%temp%"
findstr /p /A:%1 "." "!str!\..\x" nul
if /i "%~3"=="/n" echo(
exit /b

:initColorPrint
for /F "tokens=1,2 delims=#" %%a in ('"Prompt #$H#$E# & echo on & for %%b in (1) do rem"') do set "DEL=%%a"
<nul >"%temp%\x" set /p "=%DEL%%DEL%%DEL%%DEL%%DEL%%DEL%.%DEL%"
exit /b

:cleanupColorPrint
del "%temp%\x"
exit /b