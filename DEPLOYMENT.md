# Deploying HelpCovid 


## Creating a self-signed certificate

```
sudo apt install openssl
sudo mkdir -p /etc/ssl/localcerts
sudo openssl req -new -x509 -days 365 -nodes \
 -out /etc/ssl/localcerts/helpcovid.pem  \
 -keyout /etc/ssl/localcerts/helpcovid.key
sudo chmod 600 /etc/ssl/localcerts/helpcovid*
```

