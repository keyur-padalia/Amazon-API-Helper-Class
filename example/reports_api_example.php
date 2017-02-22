<?php

namespace example;
use AmazonAPIs\AmazonReportHelper;
require_once __DIR__ . DIRECTORY_SEPARATOR . '../AmazonAPIs/amazon_report_helper.class.inc';

$azn_rptObj = new AmazonReportHelper("_GET_AMAZON_FULFILLED_SHIPMENTS_DATA_",TRUE);

$report_data_array = $azn_rptObj->get_report();

print_r($report_data_array);