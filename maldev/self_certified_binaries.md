

```

openssl req -x509 -newkey rsa:4096 -keyout key.pem -out cert.pem -sha256 -days 100
openssl pkcs12 -inkey key.pem -in cert.pem -export -out sign.pfx
signtool.exe sign /f sign.pfx /p "previous-password" /t http://timestamp.digicert.com /fd sha256 binary.exe
```