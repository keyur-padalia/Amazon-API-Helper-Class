<?php

namespace examples;
use AmazonAPIs\AmazonOrderHelper;

require_once __DIR__ . DIRECTORY_SEPARATOR . '../AmazonAPI/amazon_orders_helper.class.inc';

$azn_order = new AmazonOrdersHelper();

//Call for Listing orders
$order_list = $azn_order->listOrders();

//Call for getting order's data by order id
$order_data = $azn_order->getOrderData("123-1234567-1234567");
