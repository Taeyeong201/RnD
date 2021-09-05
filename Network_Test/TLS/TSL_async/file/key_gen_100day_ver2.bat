@echo off
call :initColorPrint
title ######### 키 생성 #########
echo.
echo.

echo 현재 디렉토리에 KEY를 생성한다
cd
call :colorPrint FC "각 KEY의 Common Name은 필수 입력"
echo.
call :colorPrint FC "각 KEY의 Common Name은 필수 입력"
echo.


echo.
echo.
pause

cls

openssl genrsa -out rootCA.key
cls
echo ############# rootCA인증서 생성
call :colorPrint FC "############# Common Name 필수 입력"
echo.
call :colorPrint FC "############# Common Name 필수 입력"
echo.
call :colorPrint FC "############# Common Name 필수 입력"
echo.
call :colorPrint FC "############# Common Name 필수 입력"
echo.
echo.
openssl req -x509 -new -nodes -key rootCA.key -days 100 -out rootCA.crt

cls
openssl genrsa -out server.key 2048
cls

echo ############# Server 인증 요청서 생성
call :colorPrint FC "############# Common Name 필수 입력(user와 다른 값 필수)"
echo.
call :colorPrint FC "############# Common Name 필수 입력(user와 다른 값 필수)"
echo.
call :colorPrint FC "############# Common Name 필수 입력(user와 다른 값 필수)"
echo.
call :colorPrint FC "############# Common Name 필수 입력(user와 다른 값 필수)"
echo.
echo.
openssl req -new -key server.key -out server.csr
cls

openssl x509 -req -in server.csr -CA rootCA.crt -CAkey rootCA.key -CAcreateserial -out server.crt -days 100

openssl verify -CAfile rootCA.crt server.crt
openssl verify -CAfile rootCA.crt rootCA.crt
echo.

call :colorPrint FC "############# 오류가 정상 ##############"
echo.
call :colorPrint FC "############# 오류가 정상 ##############"
echo.
call :colorPrint FC "############# 오류가 정상 ##############"
echo.
call :colorPrint FC "############# 오류가 정상 ##############"
echo.
openssl verify -CAfile server.crt server.crt
call :colorPrint FC "############# OK 뜨면 다시 만드세요 ##############"
echo.
call :colorPrint FC "############# OK 뜨면 다시 만드세요 ##############"
echo.
call :colorPrint FC "############# OK 뜨면 다시 만드세요 ##############"
echo.
call :colorPrint FC "############# OK 뜨면 다시 만드세요 ##############"
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
echo %toyear%/%tomons%/%today% KEY 생성 완료 by KTY
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