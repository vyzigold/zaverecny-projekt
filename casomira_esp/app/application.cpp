#include <user_config.h>
#include <SmingCore/SmingCore.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "PleaseEnterSSID" // Put you SSID and Password here
	#define WIFI_PWD "PleaseEnterPass"
#endif


#define INT_PIN 13   // GPIO0
#define DALSI 14

bool pohlavi = true;


void onReceive(UdpConnection& connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort); // Declaration
HttpFirmwareUpdate airUpdater;


// UDP server
const uint16_t EchoPort = 1234;
UdpConnection udp(onReceive);


HttpServer server;

/*
float pow(int zaklad, int exponent)
{
	float vysledek = 1;
	if(exponent > 0)
	{
		for(int i = 0; i < exponent; i++)
			vysledek *= zaklad;
	}
	else if(exponent < 0)
	{
		for(int i = 0; i > exponent; i--)
			vysledek /= (float)zaklad;
	}
	return vysledek;
}

float stof(char *neco)
{
	float vysledek = 0;
	int tecka = 0;
	for(int i = 0; i < strlen(neco); i++)
	{
		if(neco[i] == '.')
		{
			tecka = i;
			break;
		}
	}

	for(int i = 0; i < strlen(neco); i++)
	{
		if(i < tecka)
			vysledek += (neco[i] - '0')*pow(10,tecka-i-1);
		if(i > tecka)
			vysledek += (neco[i] - '0')*pow(10,tecka-i);
	}
	return vysledek;
}*/

//zjisti polohu tercu
bool isReady()
{
	if(!(digitalRead(INT_PIN)||digitalRead(DALSI)))
		return false;
	return true;
}

void posliTerce()
{
	String text;
	if(isReady())
		text = "1";
	else
		text = "0";
	udp.sendStringTo(IPAddress(192, 168, 4, 2), 6666, text);
}

void posliPohlavi()
{
	String text;
	if(pohlavi)
		text = "m";
	else
		text = "z";
	udp.sendStringTo(IPAddress(192, 168, 4, 2), 6666, text);
}

void zapisCas(char* data, int delka)
{
	char *levy = new char[5];
	char *pravy = new char[5];
	char *date = new char[delka - 8];
	Serial.print("Zapis: ");
	Serial.print(data);
	Serial.print('\t');
	Serial.print(delka);
	Serial.println("\n");

	for(int i = 0; i < delka; i++)
	{
		if(i < 4)
			levy[i] = data[i];
		if(i >= 4 && i < 8)
			pravy[i-4] = data[i];
		if(i >= 8)
			date[i-8] = data[i];
	}
	levy[4] = '\0';
	pravy[4] = '\0';
	date[delka-8] = '\0';

	file_t soubor;
	if(pohlavi)
		soubor = fileOpen(String("muzi"),eFO_WriteOnly | eFO_CreateIfNotExist | eFO_Append);
	else
		soubor = fileOpen(String("zeny"),eFO_WriteOnly | eFO_CreateIfNotExist | eFO_Append);
	//zapis do file ({levy}\t{pravy};)
	fileWrite(soubor, levy, 4);
	fileWrite(soubor, ";", 1);
	fileWrite(soubor,pravy, 4);
	fileWrite(soubor, ";", 1);
	fileWrite(soubor,date, delka-9);
	fileWrite(soubor, "\n", 1);
	delete[](levy);
	levy = 0;
	pravy = 0;
	fileClose(soubor);
}



void onReceive(UdpConnection& connection, char *data, int size, IPAddress remoteIP, uint16_t remotePort)
{
	if(size <= 2)
	{
		char pismeno = data[0];
		switch(pismeno)
		{
			case 'm':
				pohlavi = true;
				break;
			case 'z':
				pohlavi = false;
				break;
			case 'p':
				posliPohlavi();
				break;
			case 't':
				posliTerce();
				break;
		}
	}
	else
	{
		zapisCas(data, size);
	}
}

void IRAM_ATTR interruptHandler()
{
	String text = "p";
	Serial.println("p");
	udp.sendStringTo(IPAddress(192, 168, 4, 2), 6666, text);
}

void IRAM_ATTR interruptHandler2()
{
	String text = "l";
	Serial.println("l");
	udp.sendStringTo(IPAddress(192, 168, 4, 2), 6666, text);
}

/*char * zmensiString(String* retezec)
{
	char *vysledek = new char[5];
	int tecka = 0;
	for(int i = 0; i < retezec->length(); i++)
	{
		if((*retezec)[i] == '.')
		{
			tecka = i;
			break;
		}
	}
	if(tecka > 2)
	{
		for(int i = 0; i < 4; i++)
			vysledek[i] = 9;
	}
	if(tecka == 1);
	{
		vysledek[0] = '0';
		vysledek[1] = (*retezec)[0];
		vysledek[2] = (*retezec)[2];
		vysledek[3] = (*retezec)[3];
	}
	if(tecka == 2)
	{
		vysledek[0] = (*retezec)[0];
		vysledek[1] = (*retezec)[1];
		vysledek[2] = (*retezec)[3];
		vysledek[3] = (*retezec)[4];
	}
	vysledek[4] = '\0';
	Serial.print("Returnuju: ");
	Serial.print(vysledek);
	Serial.println("\n");
	return vysledek;
}*/

void onIndex(HttpRequest &request, HttpResponse &response)
{
	file_t soubor;
	TemplateFileStream *tmpl;
	if(request.getPath() == "/zeny.html")
	{
		soubor = fileOpen(String("zeny"),eFO_ReadOnly);
		tmpl = new TemplateFileStream("zeny.html");
	}
	else
	{
		soubor = fileOpen(String("muzi"),eFO_ReadOnly);
		tmpl = new TemplateFileStream("index.html");
	}
	auto &vars = tmpl->variables();


	String add = "";
	String left = "";
	String right = "";
	String date = "";
	char *c = new char[1];
	while(!fileIsEOF(soubor))
	{
		fileRead(soubor,(void *) c,1);
		Serial.println(c);
		while(c[0] != ';')
		{
			left += c[0];
			Serial.print("Precteno: ");
			Serial.print(c[0]);
			Serial.println("\n");
			fileRead(soubor,(void *) c,1);
		}
		fileRead(soubor,(void *) c,1);
		while(c[0] != ';')
		{
			right += c[0];
			Serial.print("Precteno: ");
			Serial.print(c[0]);
			Serial.println("\n");
			fileRead(soubor,(void *) c,1);
		}
		fileRead(soubor,(void *) c,1);
		while(c[0] != '\n')
		{
			date += c[0];
			Serial.print("Precteno: ");
			Serial.print(c[0]);
			Serial.println("\n");
			fileRead(soubor,(void *) c,1);
		}
		if(request.getPath() == "zeny.html")
			add += "addProudari(\"zeny\",";
		else
			add += "addProudari(\"muzi\",";
			Serial.print("RIGHT: ");
			Serial.print(right);
			Serial.println("\n");
		add += right;
		add += ",";
		add += left;
			Serial.print("LEFT: ");
			Serial.print(left);
			Serial.println("\n");
		add += ",\"";
		add += date;
		add += "\");\n";
		left = "";
		right = "";
		date = "";
	}
	vars["add"] = add;
	fileClose(soubor);
	delete[](c);
	
	response.sendTemplate(tmpl); // this template object will be deleted automatically
}

void onExport(HttpRequest &request, HttpResponse &response)
{
	if(request.getQueryParameter(String("pohlavi"), String("0")) == String("0"))
		return;
	String csv("levy;pravy\n");
	file_t soubor;
	soubor = fileOpen(request.getQueryParameter(String("pohlavi"), String("0")),eFO_ReadOnly);

	char *c = new char[1];
	while(!fileIsEOF(soubor))
	{
		fileRead(soubor,(void *) c,1);
		csv += c[0];
	}
	fileClose(soubor);
	delete[](c);

	response.sendString(csv);
}


void onDelete(HttpRequest &request, HttpResponse &response)
{
	String id = request.getQueryParameter(String("id"), String("0"));
	if(id == String("0"))
		return;
	file_t soubor;
	String jmenoSouboru;
	if(id[0] == 'm')
		jmenoSouboru = "muzi";
	if(id[0] == 'z')
		jmenoSouboru = "zeny";
	fileOpen(&jmenoSouboru[0],eFO_ReadOnly);
	String nacteno = "";
	char *c = new char[1];
	while(!fileIsEOF(soubor))
	{
		fileRead(soubor,(void *) c,1);
		nacteno+=c;
	}
	fileClose(soubor);
	delete[](c);
	fileDelete(&jmenoSouboru[0]);
	soubor = fileOpen(&jmenoSouboru[0],eFO_WriteOnly | eFO_Append | eFO_CreateIfNotExist);
	int zaznam = 0;
	for(int i = 0; i < nacteno.length(); i++)
	{
		if(zaznam != id[1])
			fileWrite(soubor, (void*)&(nacteno[i]), 1);
		if(nacteno[i] == '\n')
			zaznam++;
	}
	response.setCache(86400, true); // It's important to use cache for better performance.
	response.sendFile("casy.html");
	return;
}

void onFile(HttpRequest &request, HttpResponse &response)
{
	String file = request.getPath();
	if (file[0] == '/')
		file = file.substring(1);

	if (file[0] == '.')
		response.forbidden();
	else
	{
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile(file);
	}
}

Timer downloadTimer;
HttpClient downloadClient;
int dowfid = 0;
void downloadContentFiles()
{
	if (downloadClient.isProcessing()) return; // Please, wait.

	if (downloadClient.isSuccessful())
		dowfid++; // Success. Go to next file!
	downloadClient.reset(); // Reset current download status

	if (dowfid == 0)
		downloadClient.downloadFile("192.168.4.2/index.html");
	else if (dowfid == 1)
		downloadClient.downloadFile("192.168.4.2/bootstrap.css.gz");
	else if (dowfid == 2)
		downloadClient.downloadFile("192.168.4.2/jquery.js.gz");
	else if (dowfid == 3)
		downloadClient.downloadFile("192.168.4.2/zeny.html");
	else if (dowfid == 4)
		downloadClient.downloadFile("192.168.4.2/casy.html");
	else if (dowfid == 5)
		downloadClient.downloadFile("192.168.4.2/script.js.gz");
	else if (dowfid == 6)
		downloadClient.downloadFile("192.168.4.2/update.html");
	else
	{
		// Content download was completed
		downloadTimer.stop();
	}
}

void onUpdate(HttpRequest &request, HttpResponse &response)
{
	response.sendString(request.getQueryParameter("web", String("0")));
	if(downloadTimer.isStarted())
	{
		response.sendString(String("Chvilku strpeni"));
	}
	else if(request.getQueryParameter("firmware", String("0")) == "true")
	{
		response.sendString(String("update firmware prave probiha"));
		airUpdater.start();
	}
	else if(request.getQueryParameter("web", String("0")) == "true")
	{
		Serial.println("Im in");
		fileDelete("index.html");
		fileDelete("zeny.html");
		fileDelete("casy.html");
		fileDelete("update.html");
		fileDelete("bootstrap.css.gz");
		fileDelete("jquery.js.gz");
		fileDelete("script.js.gz");

		response.sendString(String("probiha update"));

		downloadTimer.start();
	}
	else
	{
		
		response.sendFile("update.html");
	}

}

void startWebServer()
{
	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/index.html", onIndex);
	server.addPath("/zeny.html", onIndex);
	server.addPath("/export.csv", onExport);
	server.addPath("/delete", onDelete);
	server.addPath("/update", onUpdate);
	server.setDefaultHandler(onFile);

	Serial.println("\r\n=== WEB SERVER STARTED ===");
	Serial.println(WifiAccessPoint.getIP());
	Serial.println("==============================\r\n");
}


void connectOk()
{
	Serial.println("I'm CONNECTED");

	if (!fileExist("index.html") || !fileExist("bootstrap.css.gz") || !fileExist("jquery.js.gz"))
	{
		Serial.println("Namam soubory");
	}
	else
	{
		startWebServer();
	}
}

void init()
{
	spiffs_mount(); // Mount file system, in order to work with files
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true);

	WifiStation.enable(false);
	WifiAccessPoint.enable(true);
	WifiAccessPoint.config("Casomira", "", AUTH_OPEN);
	udp.listen(EchoPort);

	pinMode(INT_PIN,INPUT);	
	pinMode(DALSI,INPUT);	

	attachInterrupt(INT_PIN, interruptHandler, CHANGE);
	attachInterrupt(DALSI, interruptHandler2, CHANGE);

	connectOk();

	//Change CPU freq. to 160MHZ
	System.setCpuFrequency(eCF_160MHz);
	Serial.print("New CPU frequency is:");
	Serial.println((int)System.getCpuFrequency());

	airUpdater.addItem(0x0000, "192.168.4.2/0x00000.bin");
	airUpdater.addItem(0x9000, "192.168.4.2/0x09000.bin");

	downloadTimer.initializeMs(3000, downloadContentFiles);
}

