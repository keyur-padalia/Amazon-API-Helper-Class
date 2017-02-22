<?php

/**
 * Description of amazon_report_api
 * Author: Keyur Padalia
 */

namespace AmazonAPIs;

use MarketplaceWebService_Exception;
use MarketplaceWebService_Interface;
use MarketplaceWebService_Model_GetReportRequest;
use MarketplaceWebService_Model_GetReportListRequest;
use MarketplaceWebService_Model_GetReportRequestListRequest;
use MarketplaceWebService_Client;
use MarketplaceWebService_Model_RequestReportRequest;
use MarketplaceWebService_Model_TypeList;
use DateTimeZone;
use DateTime;
use DateInterval;
// Including Undefined Class Files
$amazonPath = realpath(__DIR__ . '/Reports/src');
set_include_path($amazonPath . PATH_SEPARATOR . get_include_path());

spl_autoload_register(function ($className)
{
    global $amazonPath;

    $filePath = str_replace('_', DIRECTORY_SEPARATOR, $className) . '.php';

    $completePath = $amazonPath . DIRECTORY_SEPARATOR . $filePath;

    if(file_exists($completePath))
    {
        include_once $completePath;

        return;
    }
});

require_once '../config.inc';

class AmazonReportHelper
{

    public $report_type;
    public $report_data_array;
    public $service;
    public $reporting_days;

    /**
     *
     * @param String $report_type ReportType enumeration
     * @param Boolean $use_cached_report true/false if set to true then it will try to get similar report generated in last 1 hours
     * @param String $reporting_days How far back reporting data you want. Please supply negative value i.e. -10,-30 etc.<br/> By default it's -7. Some report types won't go far back to 30 days.
     */
    function __construct($report_type, $use_cached_report = false, $reporting_days = "-7")
    {
        $this->report_type = $report_type;
        $this->reporting_days = $reporting_days;

        $this->report_data_array["use_caching"] = $use_cached_report;
        $this->report_data_array["done"] = FALSE;
        $this->report_data_array["report_data"] = NULL;
        $this->report_data_array["report_request_id"] = '';
        $this->report_data_array["report_id"] = '';

        $this->report_data_array["aws_access_key"] = aws_access_key_id;
        $this->report_data_array["aws_secret_access_key"] = secret_key;
        $this->report_data_array["application_name"] = application_name;
        $this->report_data_array["application_version"] = application_version;
        $this->report_data_array["seller_id"] = seller_id;
        $this->report_data_array["marketplace_id"] = marketplace_id;
        $this->report_data_array["serviceURL"] = service_url;
        $this->report_data_array["date_format"] = date_format;


        $this->report_data_array["config"] = array(
            'ServiceURL' => $this->report_data_array["serviceURL"],
            'ProxyHost' => null,
            'ProxyPort' => -1,
            'MaxErrorRetry' => 3,
        );
        $this->service = new MarketplaceWebService_Client(aws_access_key_id, secret_key, $this->report_data_array["config"], application_name, application_version);
    }

    public function get_report()
    {
        // Requesting report for given report_type
        echo "\n==============================================================";
        echo "\nRequesting Report " . $this->report_type;
        echo "\n==============================================================\n\n";

        if($this->report_data_array["use_caching"])
        {
            // Checking if there is any report available for given report_type since last 1 hour, then use it
            $reportID = $this->check_report_generated_nearby_time();
            if($reportID !== FALSE)
            {
                $this->report_data_array["report_id"] = $reportID;
                // Getting Final Report
                $request_report_data = new MarketplaceWebService_Model_GetReportRequest();
                $request_report_data->setMerchant($this->report_data_array["seller_id"]);
                $request_report_data->setReport(@fopen('php://memory', 'rw+'));
                $request_report_data->setReportId($reportID);

                $this->report_data_array["report_data"] = $this->invokeGetReport($this->service, $request_report_data);
                $this->report_data_array["done"] = TRUE;
                return $this->report_data_array["report_data"];
            }
        }

        //Didn't found any recent report, so requesting new report
        $request = new MarketplaceWebService_Model_RequestReportRequest();
        $marketplaceIdArray = array("Id" => array($this->report_data_array["marketplace_id"]));
        $request->setMarketplaceIdList($marketplaceIdArray);
        $request->setMerchant($this->report_data_array['seller_id']);
        $request->setReportType($this->report_type);

        $request->setStartDate(new DateTime("$this->reporting_days Days", new DateTimeZone('UTC')));

        $this->report_data_array["report_request_id"] = $this->invokeRequestReport($this->service, $request);


        return $this->report_data_array["report_data"];
    }

    private function check_report_generated_nearby_time()
    {
        // Getting report ID to get final report
        $request_report_list = new MarketplaceWebService_Model_GetReportListRequest();
        $request_report_list->setMerchant($this->report_data_array["seller_id"]);
        $request_report_list->setAcknowledged(false);

        $report_type_request = new MarketplaceWebService_Model_TypeList();
        $report_type_request->setType($this->report_type);
        $request_report_list->setReportTypeList($report_type_request);

        try
        {
            $reportID = '';
            $response = $this->service->getReportList($request_report_list);

            if($response->isSetGetReportListResult())
            {
                $getReportListResult = $response->getGetReportListResult();
                $reportInfoList = $getReportListResult->getReportInfoList();

                foreach($reportInfoList as $reportInfo)
                {
                    // Converting Available date time to current timezone
                    $availableDateObj = $reportInfo->getAvailableDate();
                    $availableDateObj->setTimezone(new DateTimeZone(date_default_timezone_get()));
                    $availableDate = $availableDateObj->format('Y-m-d H:i');

                    $availableDateObj->add(new DateInterval('PT2H'));

                    // Checking if the available timestamp is withing last 1 hour
                    if(strtotime($availableDate) > strtotime("-1 Hour"))
                    {
                        echo "\n====== ReportRequestId : " . $reportInfo->getReportRequestId();
                        echo "\n====== ReportId        : " . $reportInfo->getReportId();
                        echo "\n====== Available Date  : $availableDate";
                        echo "\n====== Valid Till      : " . $availableDateObj->format('Y-m-d H:i');
                        echo "\n====== ReportType      : " . $reportInfo->getReportType() . "\n";

                        return $reportInfo->getReportId();
                    }
                }
            }
            return FALSE;
        }
        catch(MarketplaceWebService_Exception $ex)
        {
            echo("Caught Exception: " . $ex->getMessage() . "\n");
            echo("Response Status Code: " . $ex->getStatusCode() . "\n");
            echo("Error Code: " . $ex->getErrorCode() . "\n");
            echo("Error Type: " . $ex->getErrorType() . "\n");
            echo("Request ID: " . $ex->getRequestId() . "\n");
            echo("XML: " . $ex->getXML() . "\n");
        }
    }

    private function invokeRequestReport(MarketplaceWebService_Interface $service, $request)
    {
        try
        {
            $response = $service->requestReport($request);

            if($response->isSetRequestReportResult())
            {
                $requestReportResult = $response->getRequestReportResult();

                if($requestReportResult->isSetReportRequestInfo())
                {
                    $reportRequestInfo = $requestReportResult->getReportRequestInfo();
                    if($reportRequestInfo->isSetReportRequestId())
                    {
                        return $reportRequestInfo->getReportRequestId();
                    }
                }
            }
        }
        catch(MarketplaceWebService_Exception $ex)
        {
            echo("Caught Exception: " . $ex->getMessage() . "\n");
            echo("Response Status Code: " . $ex->getStatusCode() . "\n");
            echo("Error Code: " . $ex->getErrorCode() . "\n");
            echo("Error Type: " . $ex->getErrorType() . "\n");
            echo("Request ID: " . $ex->getRequestId() . "\n");
            echo("XML: " . $ex->getXML() . "\n");
        }
    }

    private function invokeGetReport(MarketplaceWebService_Interface $service, $request)
    {
        try
        {
            $response = $service->getReport($request);
            return stream_get_contents($request->getReport());
        }
        catch(MarketplaceWebService_Exception $ex)
        {
            echo("Caught Exception: " . $ex->getMessage() . "\n");
            echo("Response Status Code: " . $ex->getStatusCode() . "\n");
            echo("Error Code: " . $ex->getErrorCode() . "\n");
            echo("Error Type: " . $ex->getErrorType() . "\n");
            echo("Request ID: " . $ex->getRequestId() . "\n");
            echo("XML: " . $ex->getXML() . "\n");
        }
    }
}
