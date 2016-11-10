#include <user_config.h>
#include <SmingCore/SmingCore.h>

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "PleaseEnterSSID" // Put you SSID and Password here
	#define WIFI_PWD "PleaseEnterPass"
#endif


#define INT_PIN 13   // GPIO0
#define DALSI 12

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
		if(i >= 4 && < 8)
			pravy[i-4] = data[i];
		if(i >= 8)
			date[i-8] = data[i];
	}
	levy[4] = '\0'
	pravy[4] = '\0'
	date[delka-8] = '\0'

	file_t soubor;
	if(pohlavi)
		soubor = fileOpen(String("muzi"),eFO_WriteOnly | eFO_CreateIfNotExist | eFO_Append);
	else
		soubor = fileOpen(String("zeny"),eFO_WriteOnly | eFO_CreateIfNotExist | eFO_Append);
	pravy = terc;
	//zapis do file ({levy}\t{pravy};)
	fileWrite(soubor, levy, 4);
	fileWrite(soubor, "\t", 1);
	fileWrite(soubor,pravy, 4);
	fileWrite(soubor, "\t", 1);
	fileWrite(soubor,date, delka-8);
	fileWrite(soubor, ";", 1);
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
	udp.sendStringTo(IPAddress(192, 168, 4, 2), 6666, text);
}

void IRAM_ATTR interruptHandler2()
{
	String text = "l";
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
	soubor = fileOpen(String("muzi"),eFO_ReadOnly);
	TemplateFileStream *tmpl = new TemplateFileStream("index.html");
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
		while(c[0] != '\t')
		{
			left += c[0];
			Serial.print("Precteno: ");
			Serial.print(c[0]);
			Serial.println("\n");
			fileRead(soubor,(void *) c,1);
		}
		fileRead(soubor,(void *) c,1);
		while(c[0] != '\t')
		{
			right += c[0];
			Serial.print("Precteno: ");
			Serial.print(c[0]);
			Serial.println("\n");
			fileRead(soubor,(void *) c,1);
		}
		fileRead(soubor,(void *) c,1);
		while(c[0] != ';')
		{
			date += c[0];
			Serial.print("Precteno: ");
			Serial.print(c[0]);
			Serial.println("\n");
			fileRead(soubor,(void *) c,1);
		}
		fileRead(soubor,(void *) c,1);
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
		add += ",";
		add += date;
		add += ");\n";
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
	String csv("levy;pravy\n");
	file_t soubor;
	soubor = fileOpen(request.getQueryParameter(String("pohlavi"), String("")),eFO_ReadOnly);

	char *c = new char[1];
	fileRead(soubor,(void *) c,1);
	while(!fileIsEOF(soubor))
	{
		fileRead(soubor,(void *) c,1);
		if(c[0] != ';')
		{
			if(c[0] == '\t')
				c[0] = ';';

			csv += c[0];

			if(c[0] == '\n')
				fileRead(soubor,(void *) c,1);
		}
	}
	fileClose(soubor);
	delete[](c);

	response.sendString(csv);
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

void onUpdate(HttpRequest &request, HttpResponse &response)
{
	if(request.getQueryParameter("offset", String("0")) != "0")
	{
		String offset = "0x";
		offset+=request.getQueryParameter("offset");
		airUpdater.addItem(atoi(offset.c_str()), "192.168.4.2/spiff_rom.bin");
		response.sendString(String(atoi(offset.c_str())));
		airUpdater.start();
	}
	else
	{
		response.setCache(86400, true); // It's important to use cache for better performance.
		response.sendFile("update.html");
	}

}

void startWebServer()
{
	server.listen(80);
	server.addPath("/", onIndex);
	server.addPath("/export.csv", onExport);
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
}

