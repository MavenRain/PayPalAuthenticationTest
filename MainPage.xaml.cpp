//
// MainPage.xaml.cpp
// Implementation of the MainPage class.
//

#include "pch.h"
#include "MainPage.xaml.h"
#include <map>

using namespace concurrency;
using namespace PayPalAuthenticationTest;
using namespace Platform;
using namespace Platform::Collections;
using namespace Windows::Data::Json;
using namespace Windows::Foundation;
using namespace Windows::Foundation::Collections;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Data;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::UI::Xaml::Navigation;
using namespace Windows::Security::Cryptography;
using namespace Windows::Web;
using namespace Windows::Web::Http;
using namespace Windows::Web::Http::Headers;

// The Blank Page item template is documented at http://go.microsoft.com/fwlink/?LinkId=234238

MainPage::MainPage()
{
	InitializeComponent();
}


void PayPalAuthenticationTest::MainPage::Button_Click(Platform::Object^ sender, Windows::UI::Xaml::RoutedEventArgs^ e)
{
	auto startUri = ref new Uri("https://api.sandbox.paypal.com/v1/oauth2/token");
	auto httpClient = ref new HttpClient();
	auto httpRequest = ref new HttpRequestMessage(HttpMethod::Post,startUri);
	auto iMap = ref new Map<String^, String^>();
	httpClient->DefaultRequestHeaders->Authorization = ref new HttpCredentialsHeaderValue("Basic", CryptographicBuffer::EncodeToBase64String(CryptographicBuffer::ConvertStringToBinary("AYSkoxC6MolMcRAlHJF6xWTvIWj4A6XcyAdhBSxsL8CJ1-wTo8L3L69V5A6c:ELxcaRDSmbrGcSSAGSZMiCMM4nRoufAwiXiFcfdGeuIQoziPwuMIm8d4LFC-",BinaryStringEncoding::Utf8)));
	iMap->Insert("grant_type", "client_credentials");
	httpRequest->Content = ref new HttpFormUrlEncodedContent(iMap);
	create_task(httpClient->SendRequestAsync(httpRequest)).then([](HttpResponseMessage^ httpResponseMessage)
	{
		auto responseObject = JsonObject::Parse(httpResponseMessage->Content->ToString());
		auto endUri = ref new Uri("https://api.sandbox.paypal.com/v1/payments/payment");
		auto httpClient = ref new HttpClient();
		auto httpRequest = ref new HttpRequestMessage(HttpMethod::Post, endUri);
		//---Payment construction-----------------------------------------------
		auto jsonPayment = ref new JsonObject();
		jsonPayment->Insert("intent", JsonValue::CreateStringValue("sale"));
		auto payer = ref new JsonObject();
		payer->Insert("payer_method", JsonValue::CreateStringValue("credit_card"));
		jsonPayment->Insert("payer", payer);
		auto fundingInstruments = ref new JsonArray();
		payer->Insert("funding_instruments", fundingInstruments);
		auto groupOfCards = ref new JsonObject();
		fundingInstruments->Append(groupOfCards);
		auto soleCard = ref new JsonObject();
		soleCard->Insert("type", JsonValue::CreateStringValue("visa"));
		soleCard->Insert("number", JsonValue::CreateStringValue("1234567890123456"));
		soleCard->Insert("expire_month", JsonValue::CreateStringValue("11"));
		soleCard->Insert("expire_year", JsonValue::CreateStringValue("2018"));
		soleCard->Insert("first_name", JsonValue::CreateStringValue("Oni"));
		soleCard->Insert("last_name", JsonValue::CreateStringValue("Obi"));
		groupOfCards->Insert("credit_card", soleCard);
		auto transactionsArray = ref new JsonArray();
		jsonPayment->Insert("transactions", transactionsArray);
		auto transactionsObject = ref new JsonObject();
		transactionsArray->Append(transactionsObject);
		auto transactionDetails = ref new JsonObject();
		transactionDetails->Insert("total", JsonValue::CreateStringValue("12"));
		transactionDetails->Insert("currency", JsonValue::CreateStringValue("USD"));
		transactionsObject->Insert("amount", transactionDetails);
		transactionsObject->Insert("description", JsonValue::CreateStringValue("creating a direct payment with credit card"));
		//-------------End payment construction-----------------------------------------------------------
		httpClient->DefaultRequestHeaders->Authorization = ref new HttpCredentialsHeaderValue("Bearer", responseObject->GetNamedString("access_token", ""));
		httpRequest->Content = ref new HttpStringContent(jsonPayment->Stringify());
		httpRequest->Content->Headers->ContentType = ref new HttpMediaTypeHeaderValue("application/json");
		return create_task(httpClient->SendRequestAsync(httpRequest));
	}, task_continuation_context::use_current()).then([this](HttpResponseMessage^ httpResponseMessage)
	{
		Output->Text = httpResponseMessage->StatusCode.ToString();
	},task_continuation_context::use_current());
}
