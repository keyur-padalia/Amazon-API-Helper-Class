<?php

/**
 * Description: This Amazon Orders Helper Class uses Amazon Orders API to list orders, get order by order id.
 * Author: Keyur Padalia
 */

namespace AmazonAPIs;

// Using Order API's Class
use DOMDocument;
use MarketplaceWebServiceOrders_Client;
use MarketplaceWebServiceOrders_Model_GetOrderRequest;
use MarketplaceWebServiceOrders_Model_ListOrderItemsRequest;
use MarketplaceWebServiceOrders_Exception;
use DateTime;
use DateTimeZone;

class AmazonOrdersHelper
{

    private $aws_access_key;
    private $aws_secret_access_key;
    private $application_name;
    private $application_version;
    private $seller_id;
    private $marketplace_id;
    private $config;
    public $platform;
    public $serviceURL;
    public $date_format;
    public $nickname;
    public $amazon_url;

    public function __construct()
    {
        $this->aws_access_key = $azn_access_key;
        $this->aws_secret_access_key = $azn_secret_access_key;
        $this->application_name = $azn_app_name;
        $this->application_version = $azn_app_version;
        $this->seller_id = $azn_merchant_id;
        $this->marketplace_id = $azn_marketplace_id;
        $this->platform = $azn_platform_variant;
        $this->nickname = $nickname;
        $this->date_format = "Y-m-d";

        // Setting Amazon Product URL's first Part e.g. http://www.amazon.co.uk or .de or .fr
        if($this->platform == "uk")
            $this->amazon_url = "http://www.amazon.co.uk/";
        else
            $this->amazon_url = "http://www.amazon." . $this->platform . "/";

        $this->config = array(
            'ServiceURL' => '',
            'ProxyHost' => null,
            'ProxyPort' => -1,
            'MaxErrorRetry' => 3,
        );

        //Requiring relevant classes from amazon orders api
        spl_autoload_register(function ($className)
        {
            $filePath = str_replace('_', DIRECTORY_SEPARATOR, $className) . '.php';

            set_include_path(realpath(__DIR__ . "/Orders/src"));
            $includePaths = explode(PATH_SEPARATOR, get_include_path());
            foreach($includePaths as $includePath)
            {
                if(file_exists($includePath . DIRECTORY_SEPARATOR . $filePath))
                {
                    require_once $filePath;
                    return;
                }
            }
        });
    }

    /**
     * Lists orders by looping through next token if available
     */
    public function listOrders()
    {
        $this->config['ServiceURL'] = "https://mws-eu.amazonservices.com/Orders/2013-09-01";
        $this->serviceURL = "https://mws-eu.amazonservices.com/Orders/2013-09-01";

        $service = new MarketplaceWebServiceOrders_Client($this->aws_access_key, $this->aws_secret_access_key, $this->application_name, $this->application_version, $this->config);
        $request = new \MarketplaceWebServiceOrders_Model_ListOrdersRequest();
        $request->setSellerId($this->seller_id);

        $request->setSellerId($this->seller_id);
        $request->setMarketplaceId($this->marketplace_id);
        $request->setMaxResultsPerPage(100);
        $request->setCreatedAfter(date("c", strtotime("-3 days")));

        //$request->setBuyerEmail(BUYER_EMAIL);        
        //$request->setCreatedBefore(CREATED_BEFORE);
        //MFN = Merchant Fulfillment Network
        //AFN = Amazon Fulfillment Network
        //$request->setFulfillmentChannel("MFN/AFN");
        //$request->setLastUpdatedAfter(LAST_UPDATED_AFTER);
        //$request->setLastUpdatedBefore(LAST_UPDATED_BEFORE);
        //Pending, Unshipped, PartiallyShipped, Shipped, InvoiceUnconfirmed, Canceled
        //$request->setOrderStatus(ORDER_STATUS);
        //COD = Cash on delivery
        //CVS = Convenience store payment
        //Other = Any payment method other than COD or CVS
        //$request->setPaymentMethod(PAYMENT_METHOD);
        //$request->setSellerOrderId(SELLER_ORDER_ID);
        //PendingPickUp, LabelCanceled, PickedUp, AtDestinationFC, Delivered, RejectedByBuyer, Undeliverable, ReturnedToSeller, Lost
        //$request->setTFMShipmentStatus(TFM_SHIPMENT_STATUS);
        //Getting first 100 orders and checking if the Next Token exists or not
        try
        {
            $response = $service->ListOrders($request);

            $dom = new DOMDocument();
            $dom->loadXML($response->toXML());
            $dom->preserveWhiteSpace = false;
            $dom->formatOutput = true;
            $xml_data = $dom->saveXML();
            $dom->loadXML($xml_data);

            //Order's Data Array
            $orderArray = simplexml_load_string($xml_data);

            $next_token = '';
            if(isset($orderArray->ListOrdersResult->NextToken))
            {
                $next_token = (String) $orderArray->ListOrdersResult->NextToken;
            }

            //Initializing new $request for Next_Token
            $request_next_token = new \MarketplaceWebServiceOrders_Model_ListOrdersByNextTokenRequest();
            $request_next_token->setSellerId($this->seller_id);

            $try_count = 1;
            do
            {
                $request_next_token->setNextToken($next_token);
                $next_token = "";

                try
                {
                    $response = $service->ListOrdersByNextToken($request_next_token);

                    $next_dom = new DOMDocument();
                    $next_dom->loadXML($response->toXML());
                    $next_dom->preserveWhiteSpace = false;
                    $next_dom->formatOutput = true;
                    $next_xml_data = $next_dom->saveXML();
                    $next_dom->loadXML($next_xml_data);

                    //Order's Data Array
                    $nextorderArray = simplexml_load_string($next_xml_data);

                    //Getting Next Token                    
                    if(isset($nextorderArray->ListOrdersByNextTokenResult->NextToken))
                    {
                        $next_token = (String) $nextorderArray->ListOrdersByNextTokenResult->NextToken;
                    }
                }
                catch(MarketplaceWebServiceOrders_Exception $ex)
                {
                    echo("Caught Exception: " . $ex->getMessage() . "\n");
                    echo("Response Status Code: " . $ex->getStatusCode() . "\n");
                    echo("Error Code: " . $ex->getErrorCode() . "\n");
                    echo("Error Type: " . $ex->getErrorType() . "\n");
                    echo("Request ID: " . $ex->getRequestId() . "\n");
                    echo("XML: " . $ex->getXML() . "\n");
                    echo("ResponseHeaderMetadata: " . $ex->getResponseHeaderMetadata() . "\n");
                }
            }
            while($next_token != "");
        }
        catch(MarketplaceWebServiceOrders_Exception $ex)
        {
            echo("Caught Exception: " . $ex->getMessage() . "\n");
            echo("Response Status Code: " . $ex->getStatusCode() . "\n");
            echo("Error Code: " . $ex->getErrorCode() . "\n");
            echo("Error Type: " . $ex->getErrorType() . "\n");
            echo("Request ID: " . $ex->getRequestId() . "\n");
            echo("XML: " . $ex->getXML() . "\n");
            echo("ResponseHeaderMetadata: " . $ex->getResponseHeaderMetadata() . "\n");
        }
    }

    /**
     * Getting Order's data and related items 
     */
    public function getOrderData($orderId)
    {
        $this->config['ServiceURL'] = "https://mws-eu.amazonservices.com/Orders/2013-09-01";
        $this->serviceURL = "https://mws-eu.amazonservices.com/Orders/2013-09-01";

        $service = new MarketplaceWebServiceOrders_Client($this->aws_access_key, $this->aws_secret_access_key, $this->application_name, $this->application_version, $this->config);

        // Getting Order's Detail
        $orderData = NULL;
        $request = new MarketplaceWebServiceOrders_Model_GetOrderRequest();
        $request->setSellerId($this->seller_id);
        $request->setAmazonOrderId($orderId);

        try
        {
            $response = $service->GetOrder($request);

            $dom = new DOMDocument();
            $dom->loadXML($response->toXML());
            $dom->preserveWhiteSpace = false;
            $dom->formatOutput = true;
            $xml_data = $dom->saveXML();
            $dom->loadXML($xml_data);

            $orderArray = simplexml_load_string($xml_data);

            // Check if Order Data exist else return
            if(!isset($orderArray->GetOrderResult->Orders->Order))
                return $orderData;

            //Order's data
            $order = $orderArray->GetOrderResult->Orders->Order;
	     
	    $orderData["order"]["order_id"] = $orderId;
            $orderData["order"]["payment_method"] = (String) $order->PaymentMethod;
            $orderData["order"]["purchase_date"] = (String) $order->PurchaseDate;
            $orderData["order"]["buyer_email"] = (String) $order->BuyerEmail;
            $orderData["order"]["buyer_name"] = (String) $order->BuyerName;
            $orderData["order"]["currency"] = (String) $order->OrderTotal->CurrencyCode;
            $orderData["order"]["fulfillment_channel"] = (String) $order->FulfillmentChannel;
            $orderData["order"]["number_of_items_unshipped"] = (Int) $order->NumberOfItemsUnshipped;

            $orderData["order"]["ship_service"] = (String) $order->ShipmentServiceLevelCategory;
            $orderData["order"]["ship_service_level"] = (String) $order->ShipServiceLevel;
            $orderData["order"]["recipient_name"] = (String) $order->ShippingAddress->Name;
            $orderData["order"]["ship_address_1"] = (String) $order->ShippingAddress->AddressLine1;

            if(isset($order->ShippingAddress->AddressLine2))
                $orderData["order"]["ship_address_2"] = (String) $order->ShippingAddress->AddressLine2;
            else
                $orderData["order"]["ship_address_2"] = "";

            if(isset($order->ShippingAddress->AddressLine3))
                $orderData["order"]["ship_address_3"] = (String) $order->ShippingAddress->AddressLine3;
            else
                $orderData["order"]["ship_address_3"] = "";
            $orderData["order"]["ship_city"] = (String) $order->ShippingAddress->City;

            if(isset($order->ShippingAddress->StateOrRegion))
                $orderData["order"]["ship_state"] = (String) $order->ShippingAddress->StateOrRegion;
            else
                $orderData["order"]["ship_state"] = "";
            $orderData["order"]["ship_postal_code"] = (String) $order->ShippingAddress->PostalCode;
            $orderData["order"]["ship_country"] = (String) $order->ShippingAddress->CountryCode;
            $orderData["order"]["ship_phone_number"] = (String) $order->ShippingAddress->Phone;
            $orderData["order"]["status_order"] = (String) $order->OrderStatus;
            $orderData["order"]["ship_date"] = (String) $order->LastUpdateDate;
            $orderData["order"]["subtotal"] = (Float) $order->OrderTotal->Amount;
		
            // Getting Order Item's Detail
            $request = new MarketplaceWebServiceOrders_Model_ListOrderItemsRequest();
            $request->setSellerId($this->seller_id);
            $request->setAmazonOrderId($orderId);
            $response = $service->ListOrderItems($request);

            $dom = new DOMDocument();
            $dom->loadXML($response->toXML());
            $dom->preserveWhiteSpace = false;
            $dom->formatOutput = true;
            $xml_data = $dom->saveXML();
            $dom->loadXML($xml_data);

            $orderItemArray = simplexml_load_string($xml_data);

  	    //Order's item array
            $orderItems = $orderItemArray->ListOrderItemsResult->OrderItems->OrderItem;
	    
	    if(sizeof($orderItems) < 1)
                return $orderData;

            //order item's data
            $item_counter = 0;
            foreach($orderItems as $item)
            {
                $orderData["orderItems"][$item_counter]["ASIN"] = (String) $item->ASIN;
                $orderData["orderItems"][$item_counter]["order_item_id"] = (String) $item->OrderItemId;
                $orderData["orderItems"][$item_counter]["platform_sku"] = (String) $item->SellerSKU;
                $orderData["orderItems"][$item_counter]["product_name"] = (String) $item->Title;
                $orderData["orderItems"][$item_counter]["quantity_purchased"] = (Integer) $item->QuantityOrdered;
                $orderData["orderItems"][$item_counter]["quantity_shipped"] = (Integer) $item->QuantityShipped;
                $orderData["orderItems"][$item_counter]["item_price"] = (Float) $item->ItemPrice->Amount;
                $orderData["orderItems"][$item_counter]["item_tax"] = (Float) $item->ItemTax->Amount;
                $orderData["orderItems"][$item_counter]["shipping_price"] = (Float) $item->ShippingPrice->Amount;
                $orderData["orderItems"][$item_counter]["shipping_tax"] = (Float) $item->ShippingTax->Amount;
                $orderData["orderItems"][$item_counter]["gift_wrap_price"] = (Float) $item->GiftWrapPrice->Amount;
                $orderData["orderItems"][$item_counter]["gift_wrap_tax"] = (Float) $item->GiftWrapTax->Amount;
                $orderData["orderItems"][$item_counter]["item_promotion_discount"] = (Float) $item->PromotionDiscount->Amount;

                $item_counter++;
            }
            return $orderData;
        }
        catch(MarketplaceWebServiceOrders_Exception $ex)
        {
            echo("Caught Exception: " . $ex->getMessage() . "\n");
            echo("Response Status Code: " . $ex->getStatusCode() . "\n");
            echo("Error Code: " . $ex->getErrorCode() . "\n");
            echo("Error Type: " . $ex->getErrorType() . "\n");
            echo("Request ID: " . $ex->getRequestId() . "\n");
            echo("XML: " . $ex->getXML() . "\n");
            echo("ResponseHeaderMetadata: " . $ex->getResponseHeaderMetadata() . "\n");
        }
    }
}
