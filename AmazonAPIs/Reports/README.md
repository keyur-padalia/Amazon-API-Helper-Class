##Extra features

```php
$azn_rptObj = new AmazonReportHelper($report_type,$enable_caching,$reporting_days);
$report_data_array = $azn_rptObj->get_report();
```
Using the code example above will request and get a report data for you using Amazon Reports API.

####Parameters usage
* $report_type = The ReportType enumeration which can be found [here](http://docs.developer.amazonservices.com/en_UK/reports/Reports_ReportType.html#ReportTypeCategories__FBAReports).
* $enable_caching = TRUE/FALSE, by passing 'TRUE' as second parameter, which will get similar type of report generated in last 1 hour. Useful to avoid Throttling issue.
* $reporting_days = Number of days you want to go back in report data, By default the value is '-7'.