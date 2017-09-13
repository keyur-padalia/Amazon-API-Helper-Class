[![PyPI](https://img.shields.io/badge/release%20status-beta-red.svg)]()
#Amazon API Helper Classes for PHP

##Description
This Amazon API Helper Classes for PHP helps you to use Amazon APIs, such as:
* Reports API
* Products API (*not yet in this repository*)
* Feeds API (*not yet in this repository*)
* Orders API

###Installation

* Need to create config.inc in parallel with AmazonAPIs directory with following variable declaration
```php
<?php
define('aws_access_key_id','XXXXXXXX');
define('secret_key','XXXXXXXX');
define('seller_id','XXXXXXXX');
define('marketplace_id','A1F83G8C2ARO7P');
define('application_name','MarketplaceWebServiceProducts PHP5 Library');
define('application_version','2');
define('date_format','Y-m-d');
define('service_url','https://mws-eu.amazonservices.com');
```

###Amazon MWS Endpoints

|       Region       |        Amazon MWS Endpoint        |
|--------------------|-----------------------------------|
| North America (NA) | https://mws.amazonservices.com    |
| Europe (EU)        | https://mws-eu.amazonservices.com |
| India (IN)         | https://mws.amazonservices.in     |
| China (CN)         | https://mws.amazonservices.com.cn |
| Japan (JP)         | https://mws.amazonservices.jp     |

###Amazon MWS MarketplaceIds

* North America Region

| Amazon Marketplace | MarketplaceId  |
|--------------------|----------------|
| CA                 | A2EUQ1WTGCTBG2 |
| MX                 | A1AM78C64UM0Y8 |
| US                 | ATVPDKIKX0DER  |

* Europe Region

| Amazon Marketplace | MarketplaceId  |
|--------------------|----------------|
| DE                 | A1PA6795UKMFR9 |
| ES                 | A1RKKUPIHCS9HS |
| FR                 | A13V1IB3VIYZZH |
| IT                 | APJ6JRA9NG5V4  |
| UK                 | A1F83G8C2ARO7P |

* India Region

| Amazon Marketplace | MarketplaceId |
|--------------------|---------------|
| IN                 | A21TJRUUN4KGV |

* Japan Region

| Amazon Marketplace | MarketplaceId  |
|--------------------|----------------|
| JP                 | A1VC38T7YXB528 |

* China Region

| Amazon Marketplace | MarketplaceId |
|--------------------|---------------|
| CN                 | AAHKV2X7AFYLW |

###Common HTTP error status codes

|       Error code        | HTTP status code |                                                     Description                                                      |
|-------------------------|------------------|----------------------------------------------------------------------------------------------------------------------|
| InputStreamDisconnected |              400 | There was an error reading the input stream.                                                                         |
| InvalidParameterValue   |              400 | An invalid parameter value was used, or the request size exceeded the maximum accepted size, or the request expired. |
| AccessDenied            |              401 | Access was denied.                                                                                                   |
| InvalidAccessKeyId      |              403 | An invalid AWSAccessKeyId value was used.                                                                            |
| SignatureDoesNotMatch   |              403 | The signature used does not match the server's calculated signature value.                                           |
| InvalidAddress          |              404 | An invalid API section or operation value was used, or an invalid path was used.                                     |
| InternalError           |              500 | There was an internal service failure.                                                                               |
| QuotaExceeded           |              503 | The total number of requests in an hour was exceeded.                                                                |
| RequestThrottled        |              503 | The frequency of requests was greater than allowed.                                                                  |

