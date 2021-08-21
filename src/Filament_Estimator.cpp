#include "Filament_Estimator.h"
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

extern BlynkWifi Blynk;

FILAMENT_ESTIMATOR *estimatorPointer;         //Declare a pointer to WEIGHT_ESTIMATOR
static void globalButtonHandler(Button2 &btn) // define global handler
{
    estimatorPointer->buttonHandler(btn); // calls class member handler
}
static void globalRotaryHandler(ESPRotary &rty) // define global handler
{
    estimatorPointer->rotaryHandler(rty); // calls class member handler
}
static void globalHandleStatus()
{
    estimatorPointer->handleStatus();
}
static void globalHandleFileList()
{
    estimatorPointer->handleFileList();
}
static void globalHandleGetEdit()
{
    estimatorPointer->handleGetEdit();
}
static void globalHandleFileCreate()
{
    estimatorPointer->handleFileCreate();
}
static void globalHandleFileDelete()
{
    estimatorPointer->handleFileDelete();
}
static void globalHandleFileUpload()
{
    estimatorPointer->handleFileUpload();
}
static void globalHandleNotFound()
{
    estimatorPointer->handleNotFound();
}
static void globalReplyOK()
{
    estimatorPointer->replyOK();
}

MDNSResponder::hMDNSServiceQuery hMDNSServiceQuery = 0; // The global handle of the 'http.tcp' service query in the MDNS responder
void globalMDNSServiceQueryCallback(MDNSResponder::MDNSServiceInfo serviceInfo, MDNSResponder::AnswerType answerType, bool p_bSetContent)
{
    //pass information the the inside-class method
    estimatorPointer->MDNSServiceQueryCallback(serviceInfo, answerType, p_bSetContent);
}
/*
static void globalInstallDynamicServiceQuery()
{
    if (!hMDNSServiceQuery)
    {
        hMDNSServiceQuery = MDNS.installServiceQuery("spooder", "tcp", globalMDNSServiceQueryCallback);
        if (hMDNSServiceQuery)
        {
            Serial.println(F("Dynamic service query: 'spooder.tcp' install succeeded."));
        }
        else
        {
            Serial.println(F("Dynamic service query: 'spooder.tcp' install failed."));
        }
    }
}
*/
static void globalPrintSpoodersDataset()
{
    uint16_t count = 0;
    Serial.print(F("Local spooder services found: "));
    Serial.println(MDNS.answerInfo(hMDNSServiceQuery).size());

    for (auto info : MDNS.answerInfo(hMDNSServiceQuery))
    {
        count++;
        Serial.print(String(count) + ".");
        //Serial.print(info.serviceDomain());
        if (info.hostDomainAvailable())
        {
            String s = "\tHostname: ";
            s += String(info.hostDomain());
            s += (info.hostPortAvailable()) ? (":" + String(info.hostPort())) : "";
            Serial.println(s);
        }
        if (info.IP4AddressAvailable())
        {
            String s = "\tIP4:";
            for (auto ip : info.IP4Adresses())
            {
                s += " " + ip.toString();
            }
            Serial.println(s);
        }
        if (info.txtAvailable())
        {
            String s = "\tTXT:";
            for (auto kv : info.keyValues())
            {
                s += "\t" + String(kv.first) + " : " + String(kv.second) + "\n";
            }
            Serial.println(s);
        }
    }
}

//BearSSL::CertStore certStore;
//uint16_t numCerts = 0; //number or certs read from file system
void globalPrintMemory(String s);
void globalCheckGithubTag(bool connectWiFi)
{
    /*
    globalPrintMemory("Start of globalCheckGithubTag()");

    LittleFS.begin();
    
    if (connectWiFi == true)
    {
        if ((WiFi.status() != WL_CONNECTED))
        {
            WiFi.mode(WIFI_STA);
            Serial.print("Connecting WiFi...");
            WiFi.begin();
            while ((WiFi.status() != WL_CONNECTED))
            {
                Serial.print(".");
                delay(500);
            }
            Serial.println("Ok!");
        }
    }
    if (numCerts == 0)
    {
        numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
    }

    // Serial.print(F("Number of CA certs read: "));
    //Serial.println(numCerts);
    if (numCerts == 0)
    {
        Serial.println(F("No CA certs found. Unable to establish https connection."));
        return;
    }
    Serial.println(F("Checking Github latest version:"));
    ESPOTAGitHub ESPOTAGitHub(&certStore, GHOTA_USER, GHOTA_REPO, GHOTA_CURRENT_TAG, GHOTA_BIN_FILE, GHOTA_ACCEPT_PRERELEASE);

    if (ESPOTAGitHub.checkUpgrade())
    {
        Serial.print("Update found at: ");
        Serial.println(ESPOTAGitHub.getUpgradeURL());
    }
    else
    {
        Serial.print("Error: ");
        Serial.println(ESPOTAGitHub.getLastError());
    }
    globalPrintMemory("End of globalCheckGithubTag()");
    */
    return;
}
void globalPrintMemory(String s)
{
    Serial.print(s);
    Serial.print(F(" Heap: "));
    Serial.print(ESP.getFreeHeap());
    Serial.print(F("    Block: "));
    Serial.println(ESP.getMaxFreeBlockSize());
}

FILAMENT_ESTIMATOR::FILAMENT_ESTIMATOR() : button{BUTTON_PIN},
                                           rotary{ROTARY_PIN_DT, ROTARY_PIN_CLK, stepsPerClick},
                                           display{SSD1306_ADDRESS, SSD1306_SDA_PIN, SSD1306_SCL_PIN},
                                           loadcell{HX711_DOUT_PIN, HX711_SCK_PIN}

{
}
void FILAMENT_ESTIMATOR::begin(void)
{
    begin("", "", "", "");
}
void FILAMENT_ESTIMATOR::begin(const char *ssid, const char *password, const char *hostname, const char *blynk_auth_token)
{
    Serial.println();
    //Firmware version setup
    currentVersion.major = CURRENT_VERSION_MAJOR;
    currentVersion.minor = CURRENT_VERSION_MINOR;
    currentVersion.patch = CURRENT_VERSION_PATCH;

    Serial.println("Program started.");
    Serial.print(F("Firmware version "));
    Serial.print(currentVersion.major);
    Serial.print(F("."));
    Serial.print(currentVersion.minor);
    Serial.print(F("."));
    Serial.print(currentVersion.patch);
    Serial.println();

    //Setup for OLED
    display.init();
    display.flipScreenVertically();
    display.setContrast(255);
    display.setLogBuffer(5, 30);

    Serial.println("OLED setup ok.");

    //Mount file system
    fileSystemConfig.setAutoFormat(false);
    fileSystem->setConfig(fileSystemConfig);
    fsOK = fileSystem->begin();
    if (!fsOK)
    {
        Serial.println(F("Failed to mount file system"));
    }
    else
    {
        Serial.println(F("LittleFS file system mounted."));
        //listDir("");
    }
    /*
    numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
    Serial.print(F("Number of CA certs read: "));
    Serial.println(numCerts);
    */

    displayMonoBitmap("/images/logo.bmp");

    //Setup for rotary switch
    estimatorPointer = this;
    button.setClickHandler(globalButtonHandler);
    button.setLongClickHandler(globalButtonHandler);
    button.setDoubleClickHandler(globalButtonHandler);
    button.setTripleClickHandler(globalButtonHandler);
    rotary.setLeftRotationHandler(globalRotaryHandler);
    rotary.setRightRotationHandler(globalRotaryHandler);
    Serial.println(F("Rotary Switch setup ok."));

    //Setup for settings and eeprom
    EEPROM.begin(DECLARED_EEPROM_SIZE);
    //setting.calValue = 3.14159;
    //Serial.print(F("Size of setting struct: "));
    //Serial.println((uint32_t)sizeof(setting));

    //Load eeprom data to setting
    loadToSetting();
    //validate data and reset to defaults
    bool isDirty = false;

    if (isnan(setting.calValue))
    {
        Serial.println(F("EEPROM calValue invalid, using default value."));
        setting.calValue = DEFAULT_CALIBRATION_VALUE;
        isDirty = true;
    }
    Serial.print(F("Low filament threshold = "));
    Serial.println(setting.lowFilamentThreshold);
    if (setting.lowFilamentThreshold > 9999)
    {
        Serial.println(F("Low filament threshold invalid, using default value."));
        setting.lowFilamentThreshold = DEFAULT_LOW_FILAMENT_THRESHOLD_VALUE;
        isDirty = true;
    }
    lowFilament4Digit = (setting.lowFilamentThreshold / 1000U) % 10;
    lowFilament3Digit = (setting.lowFilamentThreshold / 100U) % 10;
    lowFilament2Digit = (setting.lowFilamentThreshold / 10U) % 10;
    lowFilament1Digit = (setting.lowFilamentThreshold / 1U) % 10;

    //dumpSetting();

    //Initialize notification settings for the first time
    if (setting.notifyOnPrintStarted != 0 && setting.notifyOnPrintStarted != 1)
    {
        setting.notifyOnPrintStarted = true;
        isDirty = true;
    }
    if (setting.notifyOnPrintCompleted != 0 && setting.notifyOnPrintCompleted != 1)
    {
        setting.notifyOnPrintCompleted = true;
        isDirty = true;
    }
    if (setting.notifyOnLowFilament != 0 && setting.notifyOnLowFilament != 1)
    {
        setting.notifyOnLowFilament = true;
        isDirty = true;
    }
    if (setting.notifyOnFallOffRack != 0 && setting.notifyOnFallOffRack != 1)
    {
        setting.notifyOnFallOffRack = true;
        isDirty = true;
    }
    if (setting.notifyOnFallOffBearing != 0 && setting.notifyOnFallOffBearing != 1)
    {
        setting.notifyOnFallOffBearing = true;
        isDirty = true;
    }
    if (setting.notifyOnTangled != 0 && setting.notifyOnTangled != 1)
    {
        setting.notifyOnTangled = true;
        isDirty = true;
    }

    //Initialize services settings for the first time
    if (setting.servicesWiFi != 0 && setting.servicesWiFi != 1)
    {
        setting.servicesWiFi = true;
        isDirty = true;
    }
    if (setting.servicesMDNS != 0 && setting.servicesMDNS != 1)
    {
        setting.servicesMDNS = true;
        isDirty = true;
    }
    if (setting.servicesBLYNK != 0 && setting.servicesBLYNK != 1)
    {
        setting.servicesBLYNK = true;
        isDirty = true;
    }
    if (setting.servicesWebServer != 0 && setting.servicesWebServer != 1)
    {
        setting.servicesWebServer = true;
        isDirty = true;
    }
    if (setting.servicesArduinoOTA != 0 && setting.servicesArduinoOTA != 1)
    {
        setting.servicesArduinoOTA = true;
        isDirty = true;
    }

    if (isDirty == true)
    {
        Serial.println(F("Initializing EEPROM"));
        saveToEEPROM();
    }

    //Compare firmware version

    if (versionToNumber(currentVersion) > versionToNumber(setting.version))
    {
        Serial.println(F("Newer version number of firmware uploaded."));
        setting.version.major = currentVersion.major;
        setting.version.minor = currentVersion.minor;
        setting.version.patch = currentVersion.patch;
        saveToEEPROM();
    }
    else if (versionToNumber(currentVersion) < versionToNumber(setting.version))
    {
        Serial.println(F("Older version number of firmware uploaded."));
        setting.version.major = currentVersion.major;
        setting.version.minor = currentVersion.minor;
        setting.version.patch = currentVersion.patch;
        saveToEEPROM();
    }
    else
    {
        //Same version
    }
    if (setting.spoolHolderWeight < 0 or setting.spoolHolderWeight > 999)
    {
        Serial.print(F("EEPROM spool holder weight invalid, using default value: "));
        Serial.println(DEFAULT_SPOOL_HOLDER_WEIGHT);
        setting.spoolHolderWeight = DEFAULT_SPOOL_HOLDER_WEIGHT;
        spoolHolder3Digit = (setting.spoolHolderWeight / 100U) % 10;
        spoolHolder2Digit = (setting.spoolHolderWeight / 10U) % 10;
        spoolHolder1Digit = (setting.spoolHolderWeight / 1U) % 10;
        //Set flag, if spool holder weight is set by user in sketch, it will be stored to EEPROM.
        noSpoolHolderWeightInEEPROM = true;
    }

    //load config file
    loadConfig();
    //dumpConfig();

    Serial.print("Initializing Loadcell using ");
    Serial.println(setting.calValue);

    loadcell.begin();
    loadcell.start(stabilizingTime, true);
    loadcell.setCalFactor(setting.calValue);
    tare();

    //WiFi related
    //Disable esp8266 wifi auto connection. Connection behavior is controlled by code and user.
    WiFi.setAutoConnect(false);
    WiFi.setAutoReconnect(false);

    Serial.println("Setup completed.");

    printingStatus = STATUS_BOOT;
    printingStatusString = "STATUS_BOOT";
    setPage(PAGE_HOME);

    //Enable WiFi according to user setting
    setWifi(setting.servicesWiFi);
}
void FILAMENT_ESTIMATOR::update(void)
{
    if (enableWifi == true)
    {
        updateWifi();
    }

    button.loop();
    rotary.loop();

    if (loadcell.update() == true)
    {
        newDataReady = true;
    }
    if (newDataReady == true)
    {

        totalWeight = loadcell.getData();

        //used for development purpose
        if (emulationStarted == true)
        {
            totalWeight = emulatedWeight;
        }

        filamentWeight = totalWeight - setting.spoolHolderWeight;
    }
    updateHomepage();

    //Refresh the screen after tare message period
    if (drawOverlayFlag == true)
    {
        if (millis() - drawOverlayTimer > overlayDisplayPeriod)
        {
            drawOverlayFlag = false;
            displayPage(currentPage);
        }
    }

    if (calibrateEditDigitMode == true)
    {
        checkCalibrateEditModeTimer();
    }
    if (spoolHolderEditDigitMode == true)
    {
        checkSpoolHolderEditModeTimer();
    }
    if (setSpooderIDEditMode == true)
    {
        checkSetSpooderIDEditModeTimer();
    }
    if (lowFilamentEditDigitMode == true)
    {
        checkLowFilamentEditModeTimer();
    }

    checkConnectionStatus();
    checkConnectionDisplaySymbol();

    if (isLogging == true)
    {
        //updateLogging();
        //function moved inside updateDetection()
    }

    if (emulationStarted == true)
    {
        updateEmulation();
    }
    updateDetection();

    checkCurrentPage();
}
void FILAMENT_ESTIMATOR::setWifi(bool value)
{
    if (value == true)
    {
        if (enableWifi == false)
        {
            enableWifi = true;
            Serial.println(F("Wifi enabled."));
            WiFi.setAutoReconnect(true);
            wifiStatus = WIFI_STATUS_CONNECTING;
            connectWifi();
        }
    }
    else
    {
        if (enableWifi == true)
        {
            enableWifi = false;
            Serial.println(F("Wifi disabled."));
            WiFi.disconnect();
            WiFi.setAutoReconnect(false);
            wifiStatus = WIFI_STATUS_DISABLED;
            displayPage(currentPage); //Refresh page for WIFI symbol change
        }
    }
    globalPrintMemory("setWifi: ");
}
void FILAMENT_ESTIMATOR::updateWifi()
{
    switch (wifiStatus)
    {
    case WIFI_STATUS_CONNECTING:
    {
        if (WiFi.status() == WL_CONNECTED)
        {
            wifiStatus = WIFI_STATUS_CONNECTED;
            Serial.println(F("Wifi connected."));
            Serial.print("IP Address = ");
            Serial.println(WiFi.localIP().toString());
            Serial.print("Hostname = ");
            Serial.println(WiFi.hostname());
            displayPage(currentPage); //Refresh page for WIFI symbol change
            beginServices();
        }
        break;
    }
    case WIFI_STATUS_CONNECTED:
    {
        if (MDNS.isRunning())
        {
            MDNS.update();
        }
        if (enableBlynk == true)
        {
            Blynk.run();
        }
        if (enableWebServer == true)
        {
            webServer->handleClient();
        }
        if (enableArduinoOTA == true)
        {
            ArduinoOTA->handle();
        }
        if (enableNetworkTime == true)
        {
            updateNetworkTime();
        }

        break;
    }

    default:
        break;
    }
}
void FILAMENT_ESTIMATOR::updateNetworkTime()
{

    if (millis() - updateNetworkTimeTimer < UPDATE_NETWORK_TIME_PERIOD)
    {
        return;
    }

    //Check if the network time has been received, using year>2000
    time_t now = time(nullptr);
    tm *tmp = localtime(&now);

    if (netWorkTimeReceived == false)
    {
        if (tmp->tm_year > 100)
        {
            netWorkTimeReceived = true;
            Serial.print(F("Network time received: "));
            Serial.print(ctime(&now)); //ctime includes "\n"
        }
    }
    else //netWorkTimeReceived == false
    {
    }

    updateNetworkTimeTimer = millis();
}
void FILAMENT_ESTIMATOR::connectWifi()
{

    /* Explicitly set the ESP8266 to be a WiFi-client, otherwise, it by default,
     would try to act as both a client and an access-point and could cause
     network-issues with your other WiFi-devices on your WiFi-network. */
    WiFi.mode(WIFI_STA);
    Serial.print("Set WiFi Hostname to: ");
    Serial.println(hostname);
    WiFi.hostname(hostname); //Need this here for windows mDns to work
    WiFi.begin(wifi_ssid, wifi_password);
    Serial.print(F("Connecting to "));
    Serial.println(wifi_ssid);
}
void FILAMENT_ESTIMATOR::beginServices()
{
    //Serial.println("Check Gitgub at beginServices():");
    //globalCheckGithubTag(false);
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("WiFi not connected."));
        return;
    }
    if (setting.servicesMDNS == true)
    {
        setMDNS(true);
    }
    if (setting.servicesBLYNK == true)
    {
        setBlynk(true);
    }
    if (setting.servicesWebServer == true)
    {
        setWebServer(true);
    }
    if (setting.servicesArduinoOTA == true)
    {
        setArduinoOTA(true);
    }

    //beginArduinoOTA();
    if (enableNetworkTime == true)
    {
        configTime(TZ_Etc_GMT, "pool.ntp.org");
    }

    //Serial.println(F("NTP started."));

    Serial.println(F("Services started."));
}
void FILAMENT_ESTIMATOR::setMDNS(bool value)
{
    if (value == true) //turn on mDNS
    {
        if (WiFi.status() != WL_CONNECTED)
        {
            Serial.println(F("Failed. WiFi not connected."));
        }
        else //Wifi is connected
        {
            if (MDNS.isRunning())
            {
                Serial.println(F("mDNS already running."));
                if (MDNS.setHostname(hostname) == true)
                {
                    Serial.print(F("Hostname changed to: "));
                    Serial.println(hostname);
                }
            }
            else //MDNS is not running
            {
                if (validSpooderID == true)
                {
                    Serial.print(F("Starting mDNS as: "));
                    Serial.print(hostname);
                    if (MDNS.begin(hostname) == true)
                    {
                        Serial.println(F(" succeeded."));
                        //add service announcements
                        MDNS.addService(0, "http", "tcp", 80);                     //for web access
                        spooderService = MDNS.addService(0, "spooder", "tcp", 80); //for inter-device data exchange, using websocket
                        installDynamicServiceQuery();
                    }
                    else
                    {
                        Serial.println(" failed.");
                    }
                }
                else //no valid spooder ID
                {
                    Serial.println("Invalid spooder ID, mDNS not started.");
                }
            } //MDNS is not running
        }

    }    //turn on mDNS
    else //turn off mDNS
    {
        if (MDNS.isRunning())
        {
            hMDNSServiceQuery = 0;
            MDNS.close();
            Serial.println(F("mDNS stopped."));
        }
        else //MDNS is not running
        {
            Serial.println(F("mDNS not started."));
        }
    } //turn off mDNS
    globalPrintMemory("setMDNS: ");
    return;
}
void FILAMENT_ESTIMATOR::installDynamicServiceQuery()
{
    if (!hMDNSServiceQuery)
    {
        hMDNSServiceQuery = MDNS.installServiceQuery("spooder", "tcp", globalMDNSServiceQueryCallback);
        if (hMDNSServiceQuery)
        {
            Serial.println(F("Dynamic service query: 'spooder.tcp' install succeeded."));
        }
        else
        {
            Serial.println(F("Dynamic service query: 'spooder.tcp' install failed."));
        }
    }
}
void FILAMENT_ESTIMATOR::MDNSServiceQueryCallback(MDNSResponder::MDNSServiceInfo serviceInfo, MDNSResponder::AnswerType answerType, bool p_bSetContent)
{
    String answerInfo;
    answerInfo = String(serviceInfo.hostDomain()) + ": ";
    switch (answerType)
    {
    case MDNSResponder::AnswerType::ServiceDomain:
        answerInfo += "ServiceDomain " + String(serviceInfo.serviceDomain());
        break;
    case MDNSResponder::AnswerType::HostDomainAndPort:
        answerInfo += "HostDomainAndPort " + String(serviceInfo.hostDomain()) + ":" + String(serviceInfo.hostPort());
        break;
    case MDNSResponder::AnswerType::IP4Address:
        answerInfo += "IP4Address ";
        for (IPAddress ip : serviceInfo.IP4Adresses())
        {
            answerInfo += "- " + ip.toString();
        };
        break;
    case MDNSResponder::AnswerType::Txt:
        answerInfo += "TXT " + String(serviceInfo.strKeyValue());
        for (auto kv : serviceInfo.keyValues())
        {
            answerInfo += "\nkv : " + String(kv.first) + " : " + String(kv.second);
        }
        break;
    default:
        answerInfo = "Unknown Answertype";
    }
    Serial.printf("Answer %s %s\n", answerInfo.c_str(), p_bSetContent ? "Modified" : "Deleted");
}
void FILAMENT_ESTIMATOR::setBlynk(bool value)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("Failed. WiFi not connected."));
        return;
    }
    if (value == true) //turn on blynk
    {
        enableBlynk = true;
        if (Blynk.connected() == true)
        {
            Serial.print(F("Blynk already connected."));
        }
        else //Blynk not connected yet
        {
            Serial.print(F("Connecting Blynk using:"));
            Serial.println(blynk_auth);

            //Not using Blynk.begin(), because it
            //1.Connection to the network (WiFi, Ethernet, …)
            //2.Call of Blynk.config(...) to set Auth Token, Server Address, etc.
            //3.Attempts to connect to the server once (can block for more than 30s)
            //WiFi is controlled by code/user

            Blynk.config(blynk_auth);
            Blynk.connect(3000);

            if (Blynk.connected() == true)
            {
                Serial.println(F("Blynk connected."));
            }
            else
            {
                Serial.println(F("Blynk not connected."));
                if (Blynk.isTokenInvalid() == true)
                {
                    Serial.println(F("Blynk token is invalid."));
                }
            }
        }
    }
    else //turn off blynk
    {
        enableBlynk = false;
        if (Blynk.CONNECTED == true)
        {
            Blynk.disconnect();
            Serial.println(F("Blynk disonnected."));
        }
        else //Blynk not connected yet
        {
            Serial.print(F("Blynk not connected."));
        }
    }
    displayPage(currentPage); //Refresh page for blynk symbol change
    globalPrintMemory("setBlynk: ");
}
void FILAMENT_ESTIMATOR::setWebServer(bool value)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("Failed. WiFi not connected."));
        return;
    }
    if (value == true) //turn on web server
    {
        if (enableWebServer == true) //already enabled
        {
            Serial.println(F("Web Server already enabled."));
        }
        else //enable simple web server for file management
        {
            webServer = new ESP8266WebServer(80);

            // web server init for file system manger
            // Filesystem status
            webServer->on("/status", HTTP_GET, globalHandleStatus);

            // List directory
            webServer->on("/list", HTTP_GET, globalHandleFileList);

            // Load editor
            webServer->on("/edit", HTTP_GET, globalHandleGetEdit);

            // Create file
            webServer->on("/edit", HTTP_PUT, globalHandleFileCreate);

            // Delete file
            webServer->on("/edit", HTTP_DELETE, globalHandleFileDelete);

            // Upload file
            // - first callback is called after the request has ended with all parsed arguments
            // - second callback handles file upload at that location
            webServer->on("/edit", HTTP_POST, globalReplyOK, globalHandleFileUpload);

            // Default handler for all URIs not defined above
            // Use it to read files from filesystem
            webServer->onNotFound(globalHandleNotFound);

            // Start server
            webServer->begin();
            enableWebServer = true;
            Serial.print(F("Web Server enabled at:  "));
            Serial.print(WiFi.localIP().toString());
            if (MDNS.isRunning() == true)
            {
                Serial.print(F("  or  "));
                Serial.print(hostname);
                Serial.print(F(".local"));
            }
            Serial.println();
        }
    }    //turn on web server
    else //trun off web server
    {
        if (enableWebServer == true) //disable web server
        {
            webServer->close();
            delete webServer;
            enableWebServer = false;
            Serial.println(F("Web Server disabled."));
        }
        else //not enabled
        {
            Serial.println(F("Web Server already disabled."));
        }
    } //trun off web server
    globalPrintMemory("setServer: ");
    return;
}
void FILAMENT_ESTIMATOR::setArduinoOTA(bool value)
{
    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("WiFi not connected."));
        if (MDNS.isRunning() == false)
        {
            Serial.println(F("mDNS not running."));
        }
        return; //conditions not met
    }           //(WiFi.status() != WL_CONNECTED)
    else        //WiFi connected and mDNS running:
    {
        if (value == true) //enable arduino OTA
        {
            if (enableArduinoOTA == true) //already enabled
            {
                Serial.println(F("Arduino OTA already enabled."));
            }
            else //enable arduino OTA
            {
                ArduinoOTA = new ArduinoOTAClass;
                //Setup for OTA
                ArduinoOTA->setHostname(hostname.c_str()); //set ArduinoOTA hostname before onStart()
                ArduinoOTA->onStart([]()
                                    { Serial.println("Start"); });
                ArduinoOTA->onEnd([]()
                                  { Serial.println("\nEnd"); });
                ArduinoOTA->onProgress([](unsigned int progress, unsigned int total)
                                       { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
                ArduinoOTA->onError([](ota_error_t error)
                                    {
                                        Serial.printf("Error[%u]: ", error);
                                        if (error == OTA_AUTH_ERROR)
                                            Serial.println("Auth Failed");
                                        else if (error == OTA_BEGIN_ERROR)
                                            Serial.println("Begin Failed");
                                        else if (error == OTA_CONNECT_ERROR)
                                            Serial.println("Connect Failed");
                                        else if (error == OTA_RECEIVE_ERROR)
                                            Serial.println("Receive Failed");
                                        else if (error == OTA_END_ERROR)
                                            Serial.println("End Failed");
                                    });
                ArduinoOTA->begin();
                enableArduinoOTA = true;

                Serial.print(F("ArduinoOTA hostname: "));
                Serial.println(ArduinoOTA->getHostname());
                Serial.println(F("Arduino OTA enabled."));
            }
        }    //(value == true) //enable arduino OTA
        else //(value == false) disable arduino OTA
        {
            if (enableArduinoOTA == true) //disable arduino OTA
            {
                delete ArduinoOTA;
                enableArduinoOTA = false;
                Serial.println(F("Arduino OTA disabled."));
            }
            else //not enabled
            {
                Serial.println(F("Arduino OTA already disabled."));
            }
        } //(value == false) disable arduino OTA
    }     //WiFi connected and mDNS running:
    globalPrintMemory("setArduinoOTA: ");
}
void FILAMENT_ESTIMATOR::checkConnectionStatus()
{
    if (millis() - checkConnectionTimer < CHECK_CONNECTION_PERIOD)
        return;
    if (enableWifi == false)
    {
        connectionStatus = CONNECTION_STATUS_NONE;
        return;
    }
    if (WiFi.status() == WL_CONNECTED)
    {
        connectionStatus = CONNECTION_STATUS_WIFI_AND_INTERNET;
        //Todo: check internet connection
        //checkGithubTag();
        return;
    }
    else
    {
        connectionStatus = CONNECTION_STATUS_NO_WIFI;
    }

    checkConnectionTimer = millis();
}
void FILAMENT_ESTIMATOR::checkConnectionDisplaySymbol()
{
    switch (connectionStatus)
    {
    case CONNECTION_STATUS_NONE:
        symbolType = SYMBOL_NONE;
        break;
    case CONNECTION_STATUS_NO_WIFI:
        symbolType = SYMBOL_NO_WIFI;
        break;
    case CONNECTION_STATUS_WIFI_AND_INTERNET:
        symbolType = SYMBOL_WIFI_AND_INTERNET;
        break;
    case CONNECTION_STATUS_WIFI_NO_INTERNET:
        if (millis() - checkConnectionDisplaySymbolTimer > CHANGE_CONNECTION_SYMBOL_PERIOD)
        {
            if (symbolType == SYMBOL_WIFI_NO_INTERNET_1)
            {
                symbolType = SYMBOL_WIFI_NO_INTERNET_2;
            }
            else
            {
                symbolType = SYMBOL_WIFI_NO_INTERNET_1;
            }
            checkConnectionDisplaySymbolTimer = millis();
        }

        break;
    }
}
void FILAMENT_ESTIMATOR::buttonHandler(Button2 &btn)
{
    switch (btn.getClickType())
    {
    case SINGLE_CLICK:
        switch (currentPage)
        {
        case PAGE_MENU:
            switch (menuIndex)
            {
            case MENU_TARE:
                setPage(PAGE_TARE);
                break;
            case MENU_CALIBRATE:
                setPage(PAGE_CALIBRATE);
                break;
            case MENU_SPOOL_HOLDER_WEIGHT:
                setPage(PAGE_SPOOL_HOLDER_WEIGHT);
                break;
            case MENU_SET_SPOODER_ID:
                setPage(PAGE_SET_SPOODER_ID);
                break;
            case MENU_LOW_FILAMENT_SETUP:
                setPage(PAGE_LOW_FILAMENT_SETUP);
                break;
            case MENU_NOTIFICATION:
                setPage(PAGE_NOTIFICATION);
                break;
            case MENU_DEBUG:
                setPage(PAGE_DEBUG);
                break;
            }

            break;
        case PAGE_TARE:
            if (tareSelection == TARE_OK)
            {
                tare();
                setPage(PAGE_MENU);
            }
            else if (tareSelection == TARE_CANCEL)
            {
                //Reset the selection to ok, for next entry
                tareSelection = TARE_OK;
                setPage(PAGE_MENU);
            }
            break;
        case PAGE_HOME:
            switch (displayType)
            {
            case DISPLAY_TYPE_TOTAL:
                displayType = DISPLAY_TYPE_FILAMENT;
                break;
            case DISPLAY_TYPE_FILAMENT:
                displayType = DISPLAY_TYPE_SPOOL_HOLDER;
                break;
            case DISPLAY_TYPE_SPOOL_HOLDER:
                displayType = DISPLAY_TYPE_TOTAL;
                break;
            }
            break;
        case PAGE_CALIBRATE:
            switch (calibrateSelection)
            {
            case CALIBRATE_4_DIGIT:
            case CALIBRATE_3_DIGIT:
            case CALIBRATE_2_DIGIT:
            case CALIBRATE_1_DIGIT:
                if (calibrateEditDigitMode == false)
                {
                    calibrateEditDigitMode = true;
                    displayCalibrateDigit = false;
                    displayPage(PAGE_CALIBRATE);
                    calibrateEditModerTimer = millis();
                }
                else
                {
                    calibrateEditDigitMode = false;
                    displayCalibrateDigit = true;
                    displayPage(PAGE_CALIBRATE);
                }
                break;
            case CALIBRATE_OK:
                calibrateEditDigitMode = false;
                //Reset the selection to the fourth digit, for next entry
                calibrateSelection = CALIBRATE_4_DIGIT;
                calibrate();
                setPage(PAGE_CALIBRATE_CONFIRM);
                break;
            case CALIBRATE_CANCEL:
                calibrateEditDigitMode = false;
                //Reset the selection to the fourth digit, for next entry
                calibrateSelection = CALIBRATE_4_DIGIT;
                setPage(PAGE_MENU);
                break;
            }
            break;
        case PAGE_SPOOL_HOLDER_WEIGHT:
            switch (spoolHolderSelection)
            {

            case SPOOL_HOLDER_3_DIGIT:
            case SPOOL_HOLDER_2_DIGIT:
            case SPOOL_HOLDER_1_DIGIT:
                if (spoolHolderEditDigitMode == false)
                {
                    spoolHolderEditDigitMode = true;
                    displaySpoolHolderDigit = false;
                    displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    spoolHolderEditModerTimer = millis();
                }
                else
                {
                    spoolHolderEditDigitMode = false;
                    displaySpoolHolderDigit = true;
                    displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                }
                break;
            case SPOOL_HOLDER_OK:
                spoolHolderEditDigitMode = false;
                //Reset the selection to the third digit, for next entry
                spoolHolderSelection = SPOOL_HOLDER_3_DIGIT;
                setting.spoolHolderWeight = getSpoolHolderWeight();
                saveToEEPROM();
                setPage(PAGE_MENU);
                break;
            case SPOOL_HOLDER_CANCEL:
                spoolHolderEditDigitMode = false;
                //Reset the selection to the third digit, for next entry
                spoolHolderSelection = SPOOL_HOLDER_3_DIGIT;
                setPage(PAGE_MENU);
                break;
            default: //other than above, which means loading preset slot value into current weight
            {
                uint8_t slotIndex = spoolHolderSelection - SPOOL_HOLDER_SLOT_1;
                spoolHolder3Digit = (spoolHolderSlotWeight[slotIndex] / 100U) % 10;
                spoolHolder2Digit = (spoolHolderSlotWeight[slotIndex] / 10U) % 10;
                spoolHolder1Digit = (spoolHolderSlotWeight[slotIndex] / 1U) % 10;
                spoolHolderSelection = SPOOL_HOLDER_OK;
                displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                break;
            }
            }
            break;
        case PAGE_SET_SPOODER_ID:
            switch (setSpooderIDSelection)
            {

            case SET_SPOODER_ID_LETTER:
            case SET_SPOODER_ID_NUMBER:
                if (setSpooderIDEditMode == false)
                {
                    setSpooderIDEditMode = true;
                    displaySpooderIDDigit = false;
                    displayPage(PAGE_SET_SPOODER_ID);
                    setSpooderIDTimer = millis();
                }
                else
                {
                    setSpooderIDEditMode = false;
                    displaySpooderIDDigit = true;
                    displayPage(PAGE_SET_SPOODER_ID);
                }
                break;
            case SET_SPOODER_ID_OK:
                setSpooderIDEditMode = false;
                //Reset the selection to the letter digit, for next entry
                setSpooderIDSelection = SET_SPOODER_ID_LETTER;
                if (spooderIDLetter >= 1 && spooderIDLetter <= 26)
                {
                    if (spooderIDNumber >= 0 && spooderIDNumber <= 99)
                    {
                        //valid new spooder ID
                        setting.spooderIDLetter = spooderIDLetter;
                        setting.spooderIDNumber = spooderIDNumber;
                        setting.spooderIDSetStatus = SPOODER_ID_USER_SET;
                        saveToEEPROM();
                        validSpooderID = true;
                        hostname = "spooder" + String((char)(spooderIDLetter + 64)) + String(spooderIDNumber);
                        Serial.print("Reset hostname to: ");
                        Serial.println(hostname);
                        setMDNS(setting.servicesMDNS); //change hostname if mDNS is enabled
                    }
                }

                //saveToEEPROM();

                setPage(PAGE_MENU);
                break;
            case SET_SPOODER_ID_CANCEL:
                spoolHolderEditDigitMode = false;
                //Reset the selection to the letter digit, for next entry
                setSpooderIDSelection = SET_SPOODER_ID_LETTER;
                if (validSpooderID == false)
                {
                    //reset to invalid value for next entry
                    spooderIDLetter = 0;
                    spooderIDNumber = 0;
                }
                setPage(PAGE_MENU);
                break;
            default: //other than above, which means loading preset slot value into current weight
            {
                uint8_t slotIndex = spoolHolderSelection - SPOOL_HOLDER_SLOT_1;
                spoolHolder3Digit = (spoolHolderSlotWeight[slotIndex] / 100U) % 10;
                spoolHolder2Digit = (spoolHolderSlotWeight[slotIndex] / 10U) % 10;
                spoolHolder1Digit = (spoolHolderSlotWeight[slotIndex] / 1U) % 10;
                spoolHolderSelection = SPOOL_HOLDER_OK;
                displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                break;
            }
            }
            break;
        case PAGE_CALIBRATE_CONFIRM:

            switch (calibrateSaveSelection)
            {
            case CALIBRATE_SAVE_OK:

                setting.calValue = newCalibrationValue;
                saveToEEPROM();
                break;
            case CALIBRATE_SAVE_CANCEL:
                //Reset the selection to ok
                calibrateSaveSelection = CALIBRATE_SAVE_OK;
                break;
            }
            setPage(PAGE_MENU);
            break;
        case PAGE_LOW_FILAMENT_SETUP:
            switch (lowFilamentSelection)
            {
            case LOW_FILAMENT_4_DIGIT:
            case LOW_FILAMENT_3_DIGIT:
            case LOW_FILAMENT_2_DIGIT:
            case LOW_FILAMENT_1_DIGIT:
                if (lowFilamentEditDigitMode == false)
                {
                    lowFilamentEditDigitMode = true;
                    displayLowFilamentDigit = false;
                    displayPage(PAGE_LOW_FILAMENT_SETUP);
                    lowFilamentEditModerTimer = millis();
                }
                else
                {
                    lowFilamentEditDigitMode = false;
                    displayLowFilamentDigit = true;
                    displayPage(PAGE_LOW_FILAMENT_SETUP);
                }
                break;
            case CALIBRATE_OK:
                lowFilamentEditDigitMode = false;
                //Reset the selection to the second digit, for next entry
                lowFilamentSelection = LOW_FILAMENT_2_DIGIT;
                setting.lowFilamentThreshold = getLowFilamentThreshold();
                saveToEEPROM();
                setPage(PAGE_MENU);
                break;
            case CALIBRATE_CANCEL:
                lowFilamentEditDigitMode = false;
                //Reset the selection to the second digit, for next entry
                lowFilamentSelection = LOW_FILAMENT_2_DIGIT;
                //Revert to original value
                lowFilament4Digit = (setting.lowFilamentThreshold / 1000U) % 10;
                lowFilament3Digit = (setting.lowFilamentThreshold / 100U) % 10;
                lowFilament2Digit = (setting.lowFilamentThreshold / 10U) % 10;
                lowFilament1Digit = (setting.lowFilamentThreshold / 1U) % 10;
                setPage(PAGE_MENU);
                break;
            }
            break;
        case PAGE_NOTIFICATION:
        {
            switch (notificationMenuSelection)
            {
            case NOTIFICATION_MENU_PRINT_STARTED:
            case NOTIFICATION_MENU_PRINT_COMPLETED:
            case NOTIFICATION_MENU_LOW_FILAMENTT:
            case NOTIFICATION_MENU_FALL_OFF_RACK:
            case NOTIFICATION_MENU_FALL_OFF_BEARING:
            case NOTIFICATION_MENU_TANGLED:
                //Toogle the selected settings
                setNotificationSetting(notificationMenuSelection, !getNotificationSetting(notificationMenuSelection));
                displayPage(PAGE_NOTIFICATION);
                break;
            case NOTIFICATION_MENU_RETURN:
                notificationMenuSelection = NOTIFICATION_MENU_PRINT_STARTED;
                notificationMenuItemStartIndex = NOTIFICATION_MENU_PRINT_STARTED;
                setPage(PAGE_MENU);
                break;
            }
            break; //case PAGE_NOTIFICATION:
        }
        case PAGE_DEBUG:
            switch (debugMenuSelection)
            {
            case DEBUG_LOAD_TO_SETTING:
                loadToSetting();
                break;
            case DEBUG_SAVE_TO_EEPROM:
                saveToEEPROM();
                break;
            case DEBUG_DUMP_SETTING:
                dumpSetting();
                break;
            case DEBUG_DUMP_EEPROM:
                dumpEEPROM();
                break;
            case DEBUG_ERASE_EEPROM:
                eraseEEPROM();
                break;
            case DEBUG_LIST_DIRECTORY:
                listDir("");
                break;
            case DEBUG_LOAD_CONFIG:
                loadConfig();
                break;
            case DEBUG_DUMP_CONFIG:
                dumpConfig();
                break;
            case DEBUG_REBOOT:
                Serial.println(F("Reboot spooder."));
                ESP.restart();
                break;
            case DEBUG_BLYNK_NOTIFY:
                notify(NOTIFICATION_TEST_MESSAGE);
                break;
            case DEBUG_START_LOGGING:
                startLogging();
                drawOverlay("Start", "Logging", 1000);
                break;
            case DEBUG_STOP_LOGGING:
                stopLogging();
                drawOverlay("Stop", "Logging", 1000);
                break;
            case DEBUG_RUN_LOG_TXT:
                startEmulation();
                break;
            case DEBUG_STOP_LOG_TXT:
                stopEmulation();
                break;
            case DEBUG_TOGGLE_DETECTION_OUTPUT:
                detectionDebugOutput = !detectionDebugOutput;
                Serial.print(F("Detection output "));
                if (detectionDebugOutput == true)
                {
                    Serial.println(F("on."));
                }
                else
                {
                    Serial.println(F("off."));
                }
                break;
            case DEBUG_QUERY_MDNS:
                //drawOverlay("Query", "mDNS", 1000);
                queryMDNS();
                break;
            case DEBUG_UPDATE_SERVICE_TXT:
                //drawOverlay("Update", "Srvc Txt", 1000);
                updateServiceTxt();
                break;
            case DEBUG_PRINT_SPOODERS_DATASET:
                //drawOverlay("Print", "Spooders", 1000);
                printSpoodersDataset();
                break;
            case DEBUG_CHECK_GITHUB_TAG:
                checkGithubTag();
                break;
            case DEBUG_PRINT_MEMORY:
                globalPrintMemory("Debug print memory:");
                break;
            case DEBUG_SERVICES:
                setPage(PAGE_DEBUG_SERVICES);
                break;
            case DEBUG_RETURN:
                debugMenuSelection = DEBUG_LOAD_TO_SETTING;
                debugMenuItemStartIndex = DEBUG_LOAD_TO_SETTING;
                setPage(PAGE_MENU);
                break;
            }
            break;
        case PAGE_DEBUG_SERVICES:
        {
            switch (servicesMenuSelection)
            {
            case SERVICES_MENU_WIFI:
            {
                //Toogle the selected settings
                setServicesSetting(SERVICES_MENU_WIFI, !getServicesSetting(SERVICES_MENU_WIFI));
                displayPage(PAGE_DEBUG_SERVICES);
                setWifi(getServicesSetting(SERVICES_MENU_WIFI)); //turn WiFi on or off
                break;
            }
            case SERVICES_MENU_MDNS:
            {
                //Toogle the selected settings
                setServicesSetting(servicesMenuSelection, !getServicesSetting(servicesMenuSelection));
                displayPage(PAGE_DEBUG_SERVICES);
                setMDNS(getServicesSetting(SERVICES_MENU_MDNS)); //turn mDNS on or off
                break;
            }
            case SERVICES_MENU_BLYNK:
            {
                //Toogle the selected settings
                setServicesSetting(servicesMenuSelection, !getServicesSetting(servicesMenuSelection));
                displayPage(PAGE_DEBUG_SERVICES);
                setBlynk(getServicesSetting(SERVICES_MENU_BLYNK)); //turn blynk on or off
                break;
            }
            case SERVICES_MENU_WEB_SERVER:
            {
                //Toogle the selected settings
                setServicesSetting(servicesMenuSelection, !getServicesSetting(servicesMenuSelection));
                displayPage(PAGE_DEBUG_SERVICES);
                setWebServer(getServicesSetting(SERVICES_MENU_WEB_SERVER)); //turn web server on or off
                break;
            }
            case SERVICES_MENU_ARDUINO_OTA:
            { //Toogle the selected settings
                setServicesSetting(servicesMenuSelection, !getServicesSetting(servicesMenuSelection));
                setArduinoOTA(getServicesSetting(SERVICES_MENU_ARDUINO_OTA)); //turn arduino OTA on or off
                displayPage(PAGE_DEBUG_SERVICES);
                break;
            }

            case SERVICES_MENU_RETURN:
                servicesMenuSelection = SERVICES_MENU_WIFI;
                servicesMenuItemStartIndex = SERVICES_MENU_WIFI;
                setPage(PAGE_DEBUG);
                break;
            }
            break; //case PAGE_DEBUG_SERVICES:
        }
        }
        break; //case SINGLE_CLICK:
    case DOUBLE_CLICK:

        break;
    case TRIPLE_CLICK:

        break;
    case LONG_CLICK:
        if (currentPage == PAGE_HOME)
        {
            tare();
        }
        break;
    }
    returnToHomepageTimer = millis();
}
void FILAMENT_ESTIMATOR::rotaryHandler(ESPRotary &rty)
{
    uint8_t direction = rotary.getDirection();
    //Serial.println(direction);
    switch (direction)
    {
    case RE_LEFT:
    {
        switch (currentPage)
        {
        case PAGE_MENU:
        {
            if (menuIndex > 0)
            {
                menuIndex--;
                if (menuIndex < menuItemStartIndex)
                {
                    menuItemStartIndex--;
                }
                setPage(PAGE_MENU);
            }
            else
            {
                setPage(PAGE_INFO);
            }
            break;
        }
        case PAGE_INFO:
        {
            setPage(PAGE_HOME);
            break;
        }
        case PAGE_CALIBRATE:
        {
            switch (calibrateEditDigitMode)
            {
            case true:
                switch (calibrateSelection)
                {
                case CALIBRATE_4_DIGIT:
                    if (calibrate4Digit > 0)
                    {
                        calibrate4Digit--;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }

                    break;
                case CALIBRATE_3_DIGIT:
                    if (calibrate3Digit > 0)
                    {
                        calibrate3Digit--;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                case CALIBRATE_2_DIGIT:
                    if (calibrate2Digit > 0)
                    {
                        calibrate2Digit--;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                case CALIBRATE_1_DIGIT:
                    if (calibrate1Digit > 0)
                    {
                        calibrate1Digit--;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                }
                break;
            case false:
                if (calibrateSelection > CALIBRATE_4_DIGIT)
                {
                    calibrateSelection = calibrateSelection - 1;
                    setPage(PAGE_CALIBRATE);
                }
                break;
            }
            break;
        }
        case PAGE_TARE:
        {
            if (tareSelection == TARE_CANCEL)
            {
                tareSelection = TARE_OK;
                setPage(PAGE_TARE);
            }

            break;
        }
        case PAGE_CALIBRATE_CONFIRM:
        {
            switch (calibrateSaveSelection)
            {
            case CALIBRATE_SAVE_CANCEL:
                calibrateSaveSelection = CALIBRATE_SAVE_OK;
                displayPage(PAGE_CALIBRATE_CONFIRM);
                break;
            }
            break;
        }
        case PAGE_SPOOL_HOLDER_WEIGHT:
        {
            switch (spoolHolderEditDigitMode)
            {
            case true:
                switch (spoolHolderSelection)
                {
                case SPOOL_HOLDER_3_DIGIT:
                    if (spoolHolder3Digit > 0)
                    {
                        spoolHolder3Digit--;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                case SPOOL_HOLDER_2_DIGIT:
                    if (spoolHolder2Digit > 0)
                    {
                        spoolHolder2Digit--;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                case SPOOL_HOLDER_1_DIGIT:
                    if (spoolHolder1Digit > 0)
                    {
                        spoolHolder1Digit--;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                }
                break;
            case false:
                if (spoolHolderSelection > SPOOL_HOLDER_3_DIGIT)
                {
                    spoolHolderSelection = spoolHolderSelection - 1;
                    if (spoolHolderSelection < spoolHolderSlotStartIndex)
                    {
                        spoolHolderSlotStartIndex--;
                        if (spoolHolderSlotStartIndex < SPOOL_HOLDER_SLOT_1)
                        {
                            spoolHolderSlotStartIndex = SPOOL_HOLDER_SLOT_1;
                        }
                    }
                    setPage(PAGE_SPOOL_HOLDER_WEIGHT);
                }
                break;
            }
            break;
        }
        case PAGE_SET_SPOODER_ID:
        {
            switch (setSpooderIDEditMode)
            {
            case true:
                switch (setSpooderIDSelection)
                {
                case SET_SPOODER_ID_LETTER:
                    if (spooderIDLetter > 1)
                    {
                        spooderIDLetter--;
                        displaySpooderIDDigit = true;
                        setSpooderIDTimer = millis();
                        displayPage(PAGE_SET_SPOODER_ID);
                    }
                    break;
                case SET_SPOODER_ID_NUMBER:
                    if (spooderIDNumber > 1)
                    {
                        spooderIDNumber--;
                        displaySpooderIDDigit = true;
                        setSpooderIDTimer = millis();
                        displayPage(PAGE_SET_SPOODER_ID);
                    }
                    break;
                }
                break;
            case false:
                if (setSpooderIDSelection > SET_SPOODER_ID_LETTER)
                {
                    setSpooderIDSelection = setSpooderIDSelection - 1;
                    setPage(PAGE_SET_SPOODER_ID);
                }
                break;
            }
            break;
        }
        case PAGE_LOW_FILAMENT_SETUP:
        {
            switch (lowFilamentEditDigitMode)
            {
            case true:
                switch (lowFilamentSelection)
                {
                case LOW_FILAMENT_4_DIGIT:
                    if (lowFilament4Digit > 0)
                    {
                        lowFilament4Digit--;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }

                    break;
                case LOW_FILAMENT_3_DIGIT:
                    if (lowFilament3Digit > 0)
                    {
                        lowFilament3Digit--;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                case LOW_FILAMENT_2_DIGIT:
                    if (lowFilament2Digit > 0)
                    {
                        lowFilament2Digit--;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                case LOW_FILAMENT_1_DIGIT:
                    if (lowFilament1Digit > 0)
                    {
                        lowFilament1Digit--;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                }
                break;
            case false:
                if (lowFilamentSelection > LOW_FILAMENT_4_DIGIT)
                {
                    lowFilamentSelection = lowFilamentSelection - 1;
                    setPage(PAGE_LOW_FILAMENT_SETUP);
                }
                break;
            }
            break;
        }
        case PAGE_NOTIFICATION:
        {
            if (notificationMenuSelection > NOTIFICATION_MENU_PRINT_STARTED)
            {
                notificationMenuSelection--;
                if (notificationMenuSelection < notificationMenuItemStartIndex)
                {
                    notificationMenuItemStartIndex--;
                }
                setPage(PAGE_NOTIFICATION);
            }

            break;
        }
        case PAGE_DEBUG:
        {
            if (debugMenuSelection > DEBUG_LOAD_TO_SETTING)
            {
                debugMenuSelection--;
                if (debugMenuSelection < debugMenuItemStartIndex)
                {
                    debugMenuItemStartIndex--;
                }
                setPage(PAGE_DEBUG);
            }

            break;
        }
        case PAGE_DEBUG_SERVICES:
        {
            if (servicesMenuSelection > SERVICES_MENU_WIFI)
            {
                servicesMenuSelection--;
                if (servicesMenuSelection < servicesMenuItemStartIndex)
                {
                    servicesMenuItemStartIndex--;
                }
                setPage(PAGE_DEBUG_SERVICES);
            }
            break;
        }

        } //switch (currentPage)
        break;
    } //case RE_LEFT:
    case RE_RIGHT:
    {
        switch (currentPage)
        {
        case PAGE_HOME:
        {
            setPage(PAGE_INFO);
            break;
        }
        case PAGE_INFO:
        {
            setPage(PAGE_MENU);
            break;
        }
        case PAGE_MENU:
        {
            if (menuIndex < (numberOfMenuItems - 1))
            {
                menuIndex++;
                if (menuIndex >= menuItemStartIndex + menuItemPerPage)
                {
                    menuItemStartIndex++;
                }
                setPage(PAGE_MENU);
            }
            break;
        }
        case PAGE_TARE:
        {
            if (tareSelection == TARE_OK)
            {
                tareSelection = TARE_CANCEL;
                setPage(PAGE_TARE);
            }
            break;
        }
        case PAGE_CALIBRATE:
        {
            switch (calibrateEditDigitMode)
            {
            case true:
                switch (calibrateSelection)
                {
                case CALIBRATE_4_DIGIT:
                    if (calibrate4Digit < 9)
                    {
                        calibrate4Digit++;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }

                    break;
                case CALIBRATE_3_DIGIT:
                    if (calibrate3Digit < 9)
                    {
                        calibrate3Digit++;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                case CALIBRATE_2_DIGIT:
                    if (calibrate2Digit < 9)
                    {
                        calibrate2Digit++;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                case CALIBRATE_1_DIGIT:
                    if (calibrate1Digit < 9)
                    {
                        calibrate1Digit++;
                        displayCalibrateDigit = true;
                        calibrateEditModerTimer = millis();
                        displayPage(PAGE_CALIBRATE);
                    }
                    break;
                }
                break;
            case false:
                if (calibrateSelection < CALIBRATE_CANCEL)
                {
                    calibrateSelection = calibrateSelection + 1;
                    setPage(PAGE_CALIBRATE);
                }
                break;
            }
            break;
        }
        case PAGE_CALIBRATE_CONFIRM:
        {
            switch (calibrateSaveSelection)
            {
            case CALIBRATE_SAVE_OK:
                calibrateSaveSelection = CALIBRATE_SAVE_CANCEL;
                displayPage(PAGE_CALIBRATE_CONFIRM);
                break;
            }
            break;
        }
        case PAGE_SPOOL_HOLDER_WEIGHT:
        {
            switch (spoolHolderEditDigitMode)
            {
            case true:
                switch (spoolHolderSelection)
                {
                case SPOOL_HOLDER_3_DIGIT:
                    if (spoolHolder3Digit < 9)
                    {
                        spoolHolder3Digit++;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                case SPOOL_HOLDER_2_DIGIT:
                    if (spoolHolder2Digit < 9)
                    {
                        spoolHolder2Digit++;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                case SPOOL_HOLDER_1_DIGIT:
                    if (spoolHolder1Digit < 9)
                    {
                        spoolHolder1Digit++;
                        displaySpoolHolderDigit = true;
                        spoolHolderEditModerTimer = millis();
                        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
                    }
                    break;
                }
                break;
            case false:
                if (spoolHolderSelection < SPOOL_HOLDER_CANCEL + spoolHolderSlotSize)
                {
                    if (spoolHolderSelection != SPOOL_HOLDER_CANCEL)
                    {
                        spoolHolderSelection = spoolHolderSelection + 1;
                        if (spoolHolderSelection - spoolHolderSlotStartIndex == 3)
                        {
                            spoolHolderSlotStartIndex++;
                        }
                    }
                    else
                    {
                        if (spoolHolderSlotStartIndex == SPOOL_HOLDER_SLOT_1)
                        {
                            spoolHolderSelection = spoolHolderSelection + 1;
                            if (spoolHolderSelection - spoolHolderSlotStartIndex == 3)
                            {
                                spoolHolderSlotStartIndex++;
                            }
                        }
                        else //menu was rotated before, return to ok after pressing,then rotate right
                        {
                            spoolHolderSelection = spoolHolderSlotStartIndex;
                        }
                    }
                    setPage(PAGE_SPOOL_HOLDER_WEIGHT);
                }
                break;
            }
            break;
        }
        case PAGE_SET_SPOODER_ID:
        {
            switch (setSpooderIDEditMode)
            {
            case true:
                switch (setSpooderIDSelection)
                {
                case SET_SPOODER_ID_LETTER:
                    if (spooderIDLetter < 26)
                    {
                        spooderIDLetter++;
                        displaySpooderIDDigit = true;
                        setSpooderIDTimer = millis();
                        displayPage(PAGE_SET_SPOODER_ID);
                    }
                    break;
                case SET_SPOODER_ID_NUMBER:
                    if (spooderIDNumber < 99)
                    {
                        spooderIDNumber++;
                        displaySpooderIDDigit = true;
                        setSpooderIDTimer = millis();
                        displayPage(PAGE_SET_SPOODER_ID);
                    }
                    break;
                }
                break;
            case false:
                if (setSpooderIDSelection < SET_SPOODER_ID_CANCEL)
                {
                    setSpooderIDSelection = setSpooderIDSelection + 1;
                    setPage(PAGE_SET_SPOODER_ID);
                }
                break;
            }
            break;
        }
        case PAGE_LOW_FILAMENT_SETUP:
        {
            switch (lowFilamentEditDigitMode)
            {
            case true:
                switch (lowFilamentSelection)
                {
                case LOW_FILAMENT_4_DIGIT:
                    if (lowFilament4Digit < 9)
                    {
                        lowFilament4Digit++;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }

                    break;
                case LOW_FILAMENT_3_DIGIT:
                    if (lowFilament3Digit < 9)
                    {
                        lowFilament3Digit++;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                case LOW_FILAMENT_2_DIGIT:
                    if (lowFilament2Digit < 9)
                    {
                        lowFilament2Digit++;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                case LOW_FILAMENT_1_DIGIT:
                    if (lowFilament1Digit < 9)
                    {
                        lowFilament1Digit++;
                        displayLowFilamentDigit = true;
                        lowFilamentEditModerTimer = millis();
                        displayPage(PAGE_LOW_FILAMENT_SETUP);
                    }
                    break;
                }
                break;
            case false:
                if (lowFilamentSelection < LOW_FILAMENT_CANCEL)
                {
                    lowFilamentSelection = lowFilamentSelection + 1;
                    setPage(PAGE_LOW_FILAMENT_SETUP);
                }
                break;
            }
            break;
        }
        case PAGE_NOTIFICATION:
        {
            if (notificationMenuSelection < (numberOfNotificationMenuItems - 1))
            {
                notificationMenuSelection++;
                if (notificationMenuSelection >= notificationMenuItemStartIndex + notificationMenuItemPerPage)
                {
                    notificationMenuItemStartIndex++;
                }
                setPage(PAGE_NOTIFICATION);
            }
            break;
        }
        case PAGE_DEBUG:
        {
            if (debugMenuSelection < (numberOfDebugMenuItems - 1))
            {
                debugMenuSelection++;
                if (debugMenuSelection >= debugMenuItemStartIndex + debugMenuItemPerPage)
                {
                    debugMenuItemStartIndex++;
                }
                setPage(PAGE_DEBUG);
            }
            break;
        }
        case PAGE_DEBUG_SERVICES:
        {
            if (servicesMenuSelection < (numberOfServicesMenuItems - 1))
            {
                servicesMenuSelection++;
                if (servicesMenuSelection >= servicesMenuItemStartIndex + servicesMenuItemPerPage)
                {
                    servicesMenuItemStartIndex++;
                }
                setPage(PAGE_DEBUG_SERVICES);
            }
            break;
        }

        } //switch (currentPage)

        break;
    } //case RE_RIGHT:

    } //switch (direction)
    returnToHomepageTimer = millis();
}
bool FILAMENT_ESTIMATOR::setPage(uint8_t page)
{
    //Serial.print(F("Set page:"));
    //Serial.println(page);
    previousPage = currentPage;
    currentPage = page;
    displayPage(currentPage);

    return true;
}
void FILAMENT_ESTIMATOR::displayPage(uint8_t page)
{

    //if (page != PAGE_HOME)
    //{
    //Serial.print(F("Display page: "));
    //Serial.println(page);
    //}

    switch (page)
    {
    case PAGE_HOME:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        String h = hostname;
        h.toUpperCase();
        display.drawString(display.getWidth() / 2, 0, h);
        display.drawRect(0, 12, 128, 1);

        drawRightIndicator(0);
        //Draw display type symbol and weight

        switch (displayType)
        {
        case DISPLAY_TYPE_TOTAL:
            display.setFont(ArialMT_Plain_24);
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setColor(WHITE);
            display.fillRect(6, 22, 4, 26);
            display.fillRect(27, 22, 4, 26);
            //display.fillRect(11, 28, 15, 13);
            display.fillRect(12, 26, 3, 17);
            display.fillRect(17, 26, 3, 17);
            display.fillRect(22, 26, 3, 32);
            display.setPixel(13, 25);
            display.setPixel(18, 25);
            display.setPixel(23, 25);
            display.setPixel(13, 43);
            display.setPixel(18, 43);
            display.setPixel(23, 58);

            if (abs(totalWeight) < 9999)
            {
                int16_t displayWeight = totalWeight;
                display.drawString(90, 20, String(displayWeight));
                //Draw unit
                display.setFont(ArialMT_Plain_10);
                display.drawString(100, 32, "g");
            }

            break;
        case DISPLAY_TYPE_FILAMENT:
            display.setFont(ArialMT_Plain_24);
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setColor(WHITE);
            display.fillRect(12, 26, 3, 17);
            display.fillRect(17, 26, 3, 17);
            display.fillRect(22, 26, 3, 32);
            display.setPixel(13, 25);
            display.setPixel(18, 25);
            display.setPixel(23, 25);
            display.setPixel(13, 43);
            display.setPixel(18, 43);
            display.setPixel(23, 58);
            switch (printingStatus)
            {
            case STATUS_BOOT:
                display.drawString(112, 20, "Booting");
                break;
            case STATUS_EMPTY:
                display.drawString(104, 20, "Empty");
                break;
            case STATUS_IDLE:
                if (abs(filamentWeight) < 9999)
                {
                    display.drawString(90, 20, String(filamentWeight, 0));
                    //Draw unit
                    display.setFont(ArialMT_Plain_10);
                    display.drawString(100, 32, "g");
                }
                break;
            case STATUS_PRINTING:
                if (abs(filamentWeight) < 9999)
                {
                    display.drawString(90, 20, String(filamentWeight, 0));
                    //Draw unit
                    display.setFont(ArialMT_Plain_10);
                    display.drawString(100, 32, "g");
                }
                break;
            default:
                break;
            } //switch (printingStatus)

            break;
        case DISPLAY_TYPE_SPOOL_HOLDER:
            display.setFont(ArialMT_Plain_24);
            display.setTextAlignment(TEXT_ALIGN_RIGHT);
            display.setColor(WHITE);
            display.fillRect(6, 22, 4, 26);
            display.fillRect(27, 22, 4, 26);
            display.fillRect(11, 28, 15, 13);
            if (abs(setting.spoolHolderWeight) < 1000)
            {
                display.drawString(90, 20, String(setting.spoolHolderWeight));
                //Draw unit
                display.setFont(ArialMT_Plain_10);
                display.drawString(100, 32, "g");
            }

            break;
        }

        //drawSymbols();

        drawDisplay();
    }
    break;
    case PAGE_INFO:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "INFO");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Name: " + String(hostname));
        String f = String(setting.version.major) + "." + String(setting.version.minor) + "." + String(setting.version.patch);
        display.drawString(6, 22, "Firmware: " + f);
        display.drawString(6, 32, "CAL Value: " + String(setting.calValue));
        if (enableWifi == true)
        {
            display.drawString(6, 42, "Wifi: " + WiFi.SSID());
            display.drawString(6, 52, "IP:" + WiFi.localIP().toString());
        }

        //drawSymbols();
        drawRightIndicator(1);
        drawDisplay();
    }
    break;
    case PAGE_MENU:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "MENU");
        display.setTextAlignment(TEXT_ALIGN_LEFT);

        display.drawString(6, 12, menuTitle[menuItemStartIndex]);
        display.drawString(6, 22, menuTitle[menuItemStartIndex + 1]);
        display.drawString(6, 32, menuTitle[menuItemStartIndex + 2]);
        display.drawString(6, 42, menuTitle[menuItemStartIndex + 3]);
        display.drawString(6, 52, menuTitle[menuItemStartIndex + 4]);
        display.drawRect(0, 12, 128, 1);

        drawRightIndicator(2);

        drawLeftIndicator(menuIndex - menuItemStartIndex);

        //drawSymbols();

        drawDisplay();

        break;
    }
    case PAGE_TARE:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "TARE");
        display.drawRect(0, 12, 128, 1);

        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Remove spool and press ");
        display.drawString(6, 22, "button to tare:");
        display.drawString(40, 36, "Ok");
        display.drawString(40, 46, "Cancel");

        if (tareSelection == TARE_OK)
        {
            drawTriangle(34, 42);
        }
        else
        {
            drawTriangle(34, 52);
        }

        drawDisplay();
        break;
    }
    case PAGE_CALIBRATE:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Calibrate");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Put known weight and");
        display.drawString(6, 22, "enter its value:");

        display.setFont(ArialMT_Plain_16);
        switch (calibrateSelection)
        {
        case CALIBRATE_4_DIGIT:
            if (displayCalibrateDigit == true)
            {
                display.drawString(12, 36, String(calibrate4Digit));
            }
            display.drawString(32, 36, String(calibrate3Digit));
            display.drawString(52, 36, String(calibrate2Digit));
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(6, 45);
            break;
        case CALIBRATE_3_DIGIT:
            display.drawString(12, 36, String(calibrate4Digit));
            if (displayCalibrateDigit == true)
            {
                display.drawString(32, 36, String(calibrate3Digit));
            }
            display.drawString(52, 36, String(calibrate2Digit));
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(26, 45);
            break;
        case CALIBRATE_2_DIGIT:
            display.drawString(12, 36, String(calibrate4Digit));
            display.drawString(32, 36, String(calibrate3Digit));
            if (displayCalibrateDigit == true)
            {
                display.drawString(52, 36, String(calibrate2Digit));
            }
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(46, 45);
            break;
        case CALIBRATE_1_DIGIT:
            display.drawString(12, 36, String(calibrate4Digit));
            display.drawString(32, 36, String(calibrate3Digit));
            display.drawString(52, 36, String(calibrate2Digit));
            if (displayCalibrateDigit == true)
            {
                display.drawString(72, 36, String(calibrate1Digit));
            }
            drawTriangle(66, 45);
            break;
        case CALIBRATE_OK:
            display.drawString(12, 36, String(calibrate4Digit));
            display.drawString(32, 36, String(calibrate3Digit));
            display.drawString(52, 36, String(calibrate2Digit));
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(90, 42);
            break;
        case CALIBRATE_CANCEL:
            display.drawString(12, 36, String(calibrate4Digit));
            display.drawString(32, 36, String(calibrate3Digit));
            display.drawString(52, 36, String(calibrate2Digit));
            display.drawString(72, 36, String(calibrate1Digit));
            drawTriangle(90, 52);
            break;
        }
        display.setFont(ArialMT_Plain_10);
        display.drawString(96, 36, "Ok");
        display.drawString(96, 46, "Cancel");
        drawDisplay();
        break;
    }
    case PAGE_CALIBRATE_CONFIRM:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Calibrate - Confirm");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Old value: " + String(setting.calValue));
        display.drawString(6, 22, "New value: " + String(newCalibrationValue));
        display.drawString(6, 32, "Save data?");

        display.drawString(96, 36, "Ok");
        display.drawString(96, 46, "Cancel");
        switch (calibrateSaveSelection)
        {
        case CALIBRATE_SAVE_OK:
            drawTriangle(90, 42);
            break;
        case CALIBRATE_SAVE_CANCEL:
            drawTriangle(90, 52);
            break;
        }

        drawDisplay();
        break;
    }
    case PAGE_SPOOL_HOLDER_WEIGHT:
    { //need this because variable is defined in case
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Spool Holder Weight");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.setFont(ArialMT_Plain_16);

        uint8_t x = 22;
        uint8_t y = 16;

        switch (spoolHolderSelection)
        {
        case SPOOL_HOLDER_3_DIGIT:

            if (displaySpoolHolderDigit == true)
            {
                display.drawString(x, y, String(spoolHolder3Digit));
            }
            display.drawString(x + 20, y, String(spoolHolder2Digit));
            display.drawString(x + 40, y, String(spoolHolder1Digit));
            drawTriangle(x - 6, y + 9);
            break;
        case SPOOL_HOLDER_2_DIGIT:

            display.drawString(x, y, String(spoolHolder3Digit));
            if (displaySpoolHolderDigit == true)
            {
                display.drawString(x + 20, y, String(spoolHolder2Digit));
            }
            display.drawString(x + 40, y, String(spoolHolder1Digit));
            drawTriangle(x - 6 + 20, y + 9);
            break;
        case SPOOL_HOLDER_1_DIGIT:

            display.drawString(x, y, String(spoolHolder3Digit));
            display.drawString(x + 20, y, String(spoolHolder2Digit));
            if (displaySpoolHolderDigit == true)
            {
                display.drawString(x + 40, y, String(spoolHolder1Digit));
            }
            drawTriangle(x - 6 + 40, y + 9);
            break;
        case SPOOL_HOLDER_OK:
            display.drawString(x, y, String(spoolHolder3Digit));
            display.drawString(x + 20, y, String(spoolHolder2Digit));
            display.drawString(x + 40, y, String(spoolHolder1Digit));
            drawTriangle(90, 20);
            break;
        case SPOOL_HOLDER_CANCEL:
            display.drawString(x, y, String(spoolHolder3Digit));
            display.drawString(x + 20, y, String(spoolHolder2Digit));
            display.drawString(x + 40, y, String(spoolHolder1Digit));
            drawTriangle(90, 30);
            break;
        default:
            display.drawString(x, y, String(spoolHolder3Digit));
            display.drawString(x + 20, y, String(spoolHolder2Digit));
            display.drawString(x + 40, y, String(spoolHolder1Digit));
            break;
        }
        display.setFont(ArialMT_Plain_10);
        display.drawString(96, 14, "Ok");
        display.drawString(96, 24, "Cancel");

        //display slot menu
        if (spoolHolderSlotSize == 0)
        {
            return;
        }
        uint8_t nameX = 20;
        uint8_t nameY = 34;
        uint8_t weightX = 96;
        for (uint8_t i = 0; i < spoolHolderSlotSize; i++)
        {
            display.drawString(nameX, nameY + i * 10, String(spoolHolderSlotName[spoolHolderSlotStartIndex - SPOOL_HOLDER_SLOT_1 + i]).substring(0, 12));
            display.drawString(weightX, nameY + i * 10, String(spoolHolderSlotWeight[spoolHolderSlotStartIndex - SPOOL_HOLDER_SLOT_1 + i]));
            if (i == 2)
            {
                break;
            }
        }

        if (spoolHolderSelection > SPOOL_HOLDER_CANCEL)
        {
            uint8_t triangleIndex = spoolHolderSelection - spoolHolderSlotStartIndex;
            //Serial.print("triangleIndex = ");
            //Serial.println(triangleIndex);
            drawTriangle(nameX - 6, nameY + 6 + (triangleIndex * 10));
        }

        drawDisplay();

        break;
    }
    case PAGE_SET_SPOODER_ID:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Set Spooder ID");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);

        display.setFont(ArialMT_Plain_24);
        uint8_t x = 24;
        uint8_t y = 16;
        uint8_t ok = 62;

        uint8_t displayLetter;

        if (spooderIDLetter == 0)
        {
            displayLetter = 45; //"-"
        }
        else
        {
            displayLetter = spooderIDLetter + 64; //Starting with A
        }

        switch (setSpooderIDSelection)
        {
        case SET_SPOODER_ID_LETTER:
            if (displaySpooderIDDigit == true)
            {
                display.drawString(x, y, String((char)(displayLetter)));
            }
            if (spooderIDNumber == 0)
            {
                display.drawString(x + 24, y, "-");
            }
            else
            {
                display.drawString(x + 24, y, String(spooderIDNumber));
            }

            drawTriangle(x - 6, y + 13);

            break;
        case SET_SPOODER_ID_NUMBER:
            display.drawString(x, y, String((char)(displayLetter)));
            if (displaySpooderIDDigit == true)
            {
                if (spooderIDNumber == 0)
                {
                    display.drawString(x + 24, y, "-");
                }
                else
                {
                    display.drawString(x + 24, y, String(spooderIDNumber));
                }
            }
            drawTriangle(x - 6 + 24, y + 13);
            break;
        case SET_SPOODER_ID_OK:
            display.drawString(x, y, String((char)(displayLetter)));
            if (spooderIDNumber == 0)
            {
                display.drawString(x + 24, y, "-");
            }
            else
            {
                display.drawString(x + 24, y, String(spooderIDNumber));
            }

            drawTriangle(x - 6 + ok, y + 8);
            break;
        case SET_SPOODER_ID_CANCEL:
            display.drawString(x, y, String((char)(displayLetter)));
            if (spooderIDNumber == 0)
            {
                display.drawString(x + 24, y, "-");
            }
            else
            {
                display.drawString(x + 24, y, String(spooderIDNumber));
            }
            drawTriangle(x - 6 + ok, y + 8 + 10);
            break;
        }
        display.setFont(ArialMT_Plain_10);
        display.drawString(x + ok, y + 2, "Ok");
        display.drawString(x + ok, y + 12, "Cancel");

        if (spooderIDLetter != 0 && spooderIDNumber != 0)
        {
            display.setFont(ArialMT_Plain_16);
            String host = "spooder";
            host += String((char)(displayLetter));
            host += String(spooderIDNumber);
            host += ".local";
            display.drawString(2, 44, host);
        }
        drawDisplay();
    }
    break;
    case PAGE_LOW_FILAMENT_SETUP:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Low Filament Setup");
        display.drawRect(0, 12, 128, 1);
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, "Set filament threshold weight");
        display.drawString(6, 22, "for notification:");

        display.setFont(ArialMT_Plain_16);
        switch (lowFilamentSelection)
        {
        case LOW_FILAMENT_4_DIGIT:
            if (displayLowFilamentDigit == true)
            {
                display.drawString(12, 36, String(lowFilament4Digit));
            }
            display.drawString(32, 36, String(lowFilament3Digit));
            display.drawString(52, 36, String(lowFilament2Digit));
            display.drawString(72, 36, String(lowFilament1Digit));
            drawTriangle(6, 45);
            break;
        case LOW_FILAMENT_3_DIGIT:
            display.drawString(12, 36, String(lowFilament4Digit));
            if (displayLowFilamentDigit == true)
            {
                display.drawString(32, 36, String(lowFilament3Digit));
            }
            display.drawString(52, 36, String(lowFilament2Digit));
            display.drawString(72, 36, String(lowFilament1Digit));
            drawTriangle(26, 45);
            break;
        case LOW_FILAMENT_2_DIGIT:
            display.drawString(12, 36, String(lowFilament4Digit));
            display.drawString(32, 36, String(lowFilament3Digit));
            if (displayLowFilamentDigit == true)
            {
                display.drawString(52, 36, String(lowFilament2Digit));
            }
            display.drawString(72, 36, String(lowFilament1Digit));
            drawTriangle(46, 45);
            break;
        case LOW_FILAMENT_1_DIGIT:
            display.drawString(12, 36, String(lowFilament4Digit));
            display.drawString(32, 36, String(lowFilament3Digit));
            display.drawString(52, 36, String(lowFilament2Digit));
            if (displayLowFilamentDigit == true)
            {
                display.drawString(72, 36, String(lowFilament1Digit));
            }
            drawTriangle(66, 45);
            break;
        case LOW_FILAMENT_OK:
            display.drawString(12, 36, String(lowFilament4Digit));
            display.drawString(32, 36, String(lowFilament3Digit));
            display.drawString(52, 36, String(lowFilament2Digit));
            display.drawString(72, 36, String(lowFilament1Digit));
            drawTriangle(90, 42);
            break;
        case LOW_FILAMENT_CANCEL:
            display.drawString(12, 36, String(lowFilament4Digit));
            display.drawString(32, 36, String(lowFilament3Digit));
            display.drawString(52, 36, String(lowFilament2Digit));
            display.drawString(72, 36, String(lowFilament1Digit));
            drawTriangle(90, 52);
            break;
        }
        display.setFont(ArialMT_Plain_10);
        display.drawString(96, 36, "Ok");
        display.drawString(96, 46, "Cancel");
        drawDisplay();
        break;
    }
    case PAGE_NOTIFICATION:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Notification");
        display.drawRect(0, 12, 128, 1);

        display.setTextAlignment(TEXT_ALIGN_LEFT);

        display.drawString(6, 12, notificationMenuTitle[notificationMenuItemStartIndex]);
        display.drawString(100, 12, (getNotificationSetting(notificationMenuItemStartIndex) == true) ? "On" : "    Off");
        display.drawString(6, 22, notificationMenuTitle[notificationMenuItemStartIndex + 1]);
        display.drawString(100, 22, (getNotificationSetting(notificationMenuItemStartIndex + 1) == true) ? "On" : "    Off");
        display.drawString(6, 32, notificationMenuTitle[notificationMenuItemStartIndex + 2]);
        display.drawString(100, 32, (getNotificationSetting(notificationMenuItemStartIndex + 2) == true) ? "On" : "    Off");
        display.drawString(6, 42, notificationMenuTitle[notificationMenuItemStartIndex + 3]);
        display.drawString(100, 42, (getNotificationSetting(notificationMenuItemStartIndex + 3) == true) ? "On" : "    Off");
        display.drawString(6, 52, notificationMenuTitle[notificationMenuItemStartIndex + 4]);
        if (notificationMenuItemStartIndex + 4 != NOTIFICATION_MENU_RETURN)
        {
            display.drawString(100, 52, (getNotificationSetting(notificationMenuItemStartIndex + 4) == true) ? "On" : "    Off");
        }

        drawLeftIndicator(notificationMenuSelection - notificationMenuItemStartIndex);
        drawDisplay();
        break;
    }
    case PAGE_DEBUG:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Debug");
        display.drawRect(0, 12, 128, 1);

        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(6, 12, debugMenuTitle[debugMenuItemStartIndex]);
        display.drawString(6, 22, debugMenuTitle[debugMenuItemStartIndex + 1]);
        display.drawString(6, 32, debugMenuTitle[debugMenuItemStartIndex + 2]);
        display.drawString(6, 42, debugMenuTitle[debugMenuItemStartIndex + 3]);
        display.drawString(6, 52, debugMenuTitle[debugMenuItemStartIndex + 4]);

        drawLeftIndicator(debugMenuSelection - debugMenuItemStartIndex);
        drawDisplay();
        break;
    }
    case PAGE_DEBUG_SERVICES:
    {
        display.clear();
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_CENTER);
        display.drawString(display.getWidth() / 2, 0, "Services");
        display.drawRect(0, 12, 128, 1);

        display.setTextAlignment(TEXT_ALIGN_LEFT);

        display.drawString(6, 12, servicesMenuTitle[servicesMenuItemStartIndex]);
        display.drawString(100, 12, (getServicesSetting(servicesMenuItemStartIndex) == true) ? "On" : "    Off");
        display.drawString(6, 22, servicesMenuTitle[servicesMenuItemStartIndex + 1]);
        display.drawString(100, 22, (getServicesSetting(servicesMenuItemStartIndex + 1) == true) ? "On" : "    Off");
        display.drawString(6, 32, servicesMenuTitle[servicesMenuItemStartIndex + 2]);
        display.drawString(100, 32, (getServicesSetting(servicesMenuItemStartIndex + 2) == true) ? "On" : "    Off");
        display.drawString(6, 42, servicesMenuTitle[servicesMenuItemStartIndex + 3]);
        display.drawString(100, 42, (getServicesSetting(servicesMenuItemStartIndex + 3) == true) ? "On" : "    Off");
        display.drawString(6, 52, servicesMenuTitle[servicesMenuItemStartIndex + 4]);
        if (servicesMenuItemStartIndex + 4 != SERVICES_MENU_RETURN)
        {
            display.drawString(100, 52, (getServicesSetting(servicesMenuItemStartIndex + 4) == true) ? "On" : "    Off");
        }

        drawLeftIndicator(servicesMenuSelection - servicesMenuItemStartIndex);
        drawDisplay();
        break;
    }
    default:
        break;
    }
}
void FILAMENT_ESTIMATOR::drawBottomIndicator(uint8_t index)
{
    const uint8_t x = 48;
    const uint8_t x_step = 12;
    const uint8_t y = 59;
    uint8_t index_x = x + x_step * index;
    display.drawCircle(x, y, 2);
    display.drawCircle(x + x_step, y, 2);
    display.drawCircle(x + x_step * 2, y, 2);

    display.drawCircle(index_x, y, 1);

    display.drawCircle(index_x, y, 3);
    display.drawCircle(index_x, y, 4);
}
void FILAMENT_ESTIMATOR::drawRightIndicator(uint8_t index)
{
    const uint8_t x = 122;
    const uint8_t y_step = 10;
    const uint8_t y_start = 20;
    uint8_t index_y = y_start + y_step * index;
    display.fillCircle(x, y_start, 2);
    display.fillCircle(x, y_start + y_step, 2);

    for (uint8_t dy = 0; dy < (numberOfMenuItems * 2); dy++)
    {
        uint8_t y = y_start + y_step * 2 + dy;
        display.fillCircle(x, y, 2);
    }

    if (index < 2)
    {
        display.fillCircle(x, index_y, 5);
    }
    else
    {
        uint8_t y = y_start + y_step * 2 + menuIndex * 2;
        display.fillCircle(x, y, 5);
    }
}
void FILAMENT_ESTIMATOR::drawLeftIndicator(uint8_t index)
{
    uint8_t x = 0;
    uint8_t y = 18 + index * 10;
    drawTriangle(x, y);
}
void FILAMENT_ESTIMATOR::drawTriangle(uint8_t x, uint8_t y)
{
    display.drawLine(x, y - 3, x, y + 3);
    display.drawLine(x + 1, y - 3, x + 1, y + 3);
    display.drawLine(x + 2, y - 2, x + 2, y + 2);
    display.drawLine(x + 3, y - 1, x + 3, y + 1);
    display.drawLine(x + 4, y, x + 4, y);
}
void FILAMENT_ESTIMATOR::drawSymbols()
{
    //Draw WiFi and Blynk symbols
    uint8_t x = 117;
    uint8_t y = 0;
    switch (symbolType)
    {
    case SYMBOL_NONE: //wifi off
        break;
    case SYMBOL_NO_WIFI: //wifi on but not connected
        break;
    case SYMBOL_WIFI_AND_INTERNET: //wifi and internet connected
        display.drawHorizontalLine(x + 2, y + 1, 6);
        display.drawHorizontalLine(x + 1, y + 2, 8);
        display.drawHorizontalLine(x, y + 3, 3);
        display.drawHorizontalLine(x + 7, y + 3, 3);
        display.drawHorizontalLine(x, y + 4, 2);
        display.drawHorizontalLine(x + 8, y + 4, 2);
        display.setPixel(x, y + 5);
        display.setPixel(x + 9, y + 5);
        display.drawHorizontalLine(x + 3, y + 5, 4);
        display.drawHorizontalLine(x + 2, y + 6, 6);
        display.setPixel(x + 2, y + 7);
        display.setPixel(x + 7, y + 7);
        display.drawHorizontalLine(x + 4, y + 8, 2);
        display.drawHorizontalLine(x + 4, y + 9, 2);
        break;
    case SYMBOL_WIFI_NO_INTERNET_1: //wifi but no internet flashing icon 1
        display.drawVerticalLine(x, y + 3, 3);
        display.drawVerticalLine(x + 9, y + 3, 3);
        display.drawVerticalLine(x + 1, y + 2, 3);
        display.drawVerticalLine(x + 8, y + 2, 3);
        display.drawVerticalLine(x + 2, y + 6, 2);
        display.drawVerticalLine(x + 7, y + 6, 2);
        display.fillRect(x + 3, y, 4, 5);
        display.fillRect(x + 4, y + 5, 2, 2);

        display.drawHorizontalLine(x + 4, y + 8, 2);
        display.drawHorizontalLine(x + 4, y + 9, 2);

        break;
    case SYMBOL_WIFI_NO_INTERNET_2: //wifi but no internet flashing icon 2
        display.drawVerticalLine(x, y + 3, 3);
        display.drawVerticalLine(x + 9, y + 3, 3);
        display.drawVerticalLine(x + 1, y + 2, 3);
        display.drawVerticalLine(x + 8, y + 2, 3);
        display.drawVerticalLine(x + 2, y + 6, 2);
        display.drawVerticalLine(x + 7, y + 6, 2);

        break;
    default:
        break;
    }
    //Blynk status
    if (Blynk.connected() == true)
    {
        display.setFont(ArialMT_Plain_10);
        display.setTextAlignment(TEXT_ALIGN_LEFT);

        display.fillRect(104 + 1, 1 + 1, 11 - 2, 10 - 1);
        display.drawHorizontalLine(104 + 2, 1, 7);
        display.drawVerticalLine(104, 3, 7);
        display.drawVerticalLine(104 + 10, 3, 7);
        display.setColor(BLACK);
        display.drawString(106, 0, "B");
        display.setColor(WHITE);
    }
}
void FILAMENT_ESTIMATOR::tare()
{
    loadcell.tareNoDelay();
    drawOverlay("Tare", "", 1000);
    Serial.println("Tare");
}
void FILAMENT_ESTIMATOR::calibrate()
{
    //Draw the overlay
    display.setColor(BLACK);
    display.fillRect(16 - 8, 12 - 4, 96 + 18, 40 + 8);
    display.setColor(WHITE);
    display.drawRect(16, 12, 96, 40);
    display.setFont(ArialMT_Plain_16);
    display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
    display.drawString(64, 32, "Calibrating...");
    drawDisplay();
    //refresh the dataset to be sure that the known mass is measured correct
    loadcell.refreshDataSet();
    calibrationWeight = getCalibrationWeight();
    Serial.print("Calibration weight = ");
    Serial.println(calibrationWeight);
    //get the new calibration value
    newCalibrationValue = loadcell.getNewCalibration(calibrationWeight);
    Serial.print("New calibration value = ");
    Serial.println(newCalibrationValue);
    Serial.println("Calibrate completed.");
}
void FILAMENT_ESTIMATOR::updateHomepage()
{
    if (millis() - updateHomepageTimer < UPDATE_HOMEPAGE_PERIOD)
        return;
    updateHomepageTimer = millis();
    if (currentPage == PAGE_HOME)
    {
        displayPage(PAGE_HOME);
    }
}
void FILAMENT_ESTIMATOR::checkCurrentPage()
{
    if (millis() - returnToHomepageTimer < RETURN_TO_HOMEPAGE_PERIOD)
        return;
    if (currentPage == PAGE_HOME)
        return;
    if (calibrateEditDigitMode == true)
        return;
    if (lowFilamentEditDigitMode == true)
        return;
    if (spoolHolderEditDigitMode == true)
        return;
    if (setSpooderIDEditMode == true)
        return;
    setPage(PAGE_HOME);
}
void FILAMENT_ESTIMATOR::drawOverlay(const char *msgLine1, const char *msgLine2, uint16_t period)
{
    overlayMsgLine1 = msgLine1;
    overlayMsgLine2 = msgLine2;
    overlayDisplayPeriod = period;
    drawOverlayFlag = true;
    drawOverlayTimer = millis();
    drawDisplay();
}
void FILAMENT_ESTIMATOR::updateOverlay()
{
    if (drawOverlayFlag == true)
    {

        display.setColor(BLACK);
        display.fillRect(6, 6, 127 - 6, 64 - 6);
        display.setColor(WHITE);
        display.drawRect(8, 8, 127 - 8, 64 - 8);
        display.setFont(ArialMT_Plain_24);
        display.setTextAlignment(TEXT_ALIGN_CENTER_BOTH);
        if (String(overlayMsgLine2).length() == 0)
        {
            display.drawString(64, 35, overlayMsgLine1);
        }
        else
        {
            display.drawString(64, 24, overlayMsgLine1);
            display.drawString(64, 48, overlayMsgLine2);
        }

        //display.drawString(64, 52, msg);
    }
}
void FILAMENT_ESTIMATOR::drawDisplay()
{
    updateOverlay();
    drawSymbols();
    display.display();
}
void FILAMENT_ESTIMATOR::checkCalibrateEditModeTimer()
{
    if (millis() - calibrateEditModerTimer > CALIBRATE_EDIT_MODE_PERIOD)
    {
        displayCalibrateDigit = !displayCalibrateDigit;
        calibrateEditModerTimer = millis();
        displayPage(PAGE_CALIBRATE);
    }
}
void FILAMENT_ESTIMATOR::checkSpoolHolderEditModeTimer()
{
    if (millis() - spoolHolderEditModerTimer > SPOOL_HOLDER_EDIT_MODE_PERIOD)
    {
        displaySpoolHolderDigit = !displaySpoolHolderDigit;
        spoolHolderEditModerTimer = millis();
        displayPage(PAGE_SPOOL_HOLDER_WEIGHT);
    }
}
void FILAMENT_ESTIMATOR::checkSetSpooderIDEditModeTimer()
{
    if (millis() - setSpooderIDTimer > SET_SPOODER_ID_EDIT_MODE_PERIOD)
    {
        displaySpooderIDDigit = !displaySpooderIDDigit;
        setSpooderIDTimer = millis();
        displayPage(PAGE_SET_SPOODER_ID);
    }
}
void FILAMENT_ESTIMATOR::checkLowFilamentEditModeTimer()
{
    if (millis() - lowFilamentEditModerTimer > LOW_FILAMENT_EDIT_MODE_PERIOD)
    {
        displayLowFilamentDigit = !displayLowFilamentDigit;
        lowFilamentEditModerTimer = millis();
        displayPage(PAGE_LOW_FILAMENT_SETUP);
    }
}
void FILAMENT_ESTIMATOR::setCalibrationWeight(uint16_t weight)
{
    if (weight > 9999)
        weight = 9999;
    calibrate4Digit = (weight / 1000U) % 10;
    calibrate3Digit = (weight / 100U) % 10;
    calibrate2Digit = (weight / 10U) % 10;
    calibrate1Digit = (weight / 1U) % 10;
}
uint16_t FILAMENT_ESTIMATOR::getCalibrationWeight()
{
    uint16_t weight = calibrate4Digit * 1000 + calibrate3Digit * 100 + calibrate2Digit * 10 + calibrate1Digit;
    return weight;
}
void FILAMENT_ESTIMATOR::setCurrentSpoolHolderWeight(uint16_t weight)
{
    if (weight > 999)
    {
        weight = 999;
    }
    if (noSpoolHolderWeightInEEPROM == true)
    {
        Serial.print(F("Current spool holder weight set to "));
        Serial.println(weight);
        setting.spoolHolderWeight = weight;
        Serial.println(F("Saving spool holder weight to EEPROM:"));
        saveToEEPROM();
        spoolHolder3Digit = (weight / 100U) % 10;
        spoolHolder2Digit = (weight / 10U) % 10;
        spoolHolder1Digit = (weight / 1U) % 10;
    }
    else
    {
        Serial.println(F("Spool holder weight exists in EEPROM. Not modified by this function."));
    }
}
uint16_t FILAMENT_ESTIMATOR::getSpoolHolderWeight()
{
    uint16_t weight = spoolHolder3Digit * 100 + spoolHolder2Digit * 10 + spoolHolder1Digit;
    return weight;
}
uint16_t FILAMENT_ESTIMATOR::getLowFilamentThreshold()
{
    uint16_t weight = lowFilament4Digit * 1000 + lowFilament3Digit * 100 + lowFilament2Digit * 10 + lowFilament1Digit;
    return weight;
}
void FILAMENT_ESTIMATOR::setStepsPerClick(uint8_t steps)
{
    stepsPerClick = steps;
    rotary.setStepsPerClick(steps);
}
uint8_t FILAMENT_ESTIMATOR::getStepsPerClick()
{

    return rotary.getStepsPerClick();
}
void FILAMENT_ESTIMATOR::setDebounceTime(uint16_t ms)
{
    button.setDebounceTime(ms);
}
void FILAMENT_ESTIMATOR::setLongClickTime(uint16_t ms)
{
    button.setLongClickTime(ms);
}
void FILAMENT_ESTIMATOR::setDoubleClickTime(uint16_t ms)
{
    button.setDoubleClickTime(ms);
}
uint16_t FILAMENT_ESTIMATOR::getDebounceTime()
{
    return button.getDebounceTime();
}
uint16_t FILAMENT_ESTIMATOR::getLongClickTime()
{
    return button.getLongClickTime();
}
uint16_t FILAMENT_ESTIMATOR::getDoubleClickTime()
{
    return button.getDoubleClickTime();
}
void FILAMENT_ESTIMATOR::loadToSetting()
{
    Serial.println(F("Load EEPROM data to setting:"));
    EEPROM.get(EEPROM_START_ADDRESS, setting);
    spoolHolder3Digit = (setting.spoolHolderWeight / 100U) % 10;
    spoolHolder2Digit = (setting.spoolHolderWeight / 10U) % 10;
    spoolHolder1Digit = (setting.spoolHolderWeight / 1U) % 10;

    switch (setting.spooderIDSetStatus)
    {
    case SPOODER_ID_USER_SET:
    case SPOODER_ID_SYSTEM_SET:
        spooderIDLetter = setting.spooderIDLetter;
        spooderIDNumber = setting.spooderIDNumber;
        validSpooderID = true;
        hostname = "spooder" + String((char)(spooderIDLetter + 64)) + String(spooderIDNumber);
        break;
    default:
        Serial.println(F("Invalid spooder ID in EEPROM."));
        break;
    }
}
void FILAMENT_ESTIMATOR::saveToEEPROM()
{
    Serial.println(F("Save setting data to EEPROM:"));
    EEPROM.put(EEPROM_START_ADDRESS, setting);
    EEPROM.commit();
}
void FILAMENT_ESTIMATOR::dumpSetting()
{
    Serial.println(F("Dump setting:"));
    //Starting from v0.3.0
    Serial.print(F("  - Firmware version: "));
    Serial.print(setting.version.major);
    Serial.print(F("."));
    Serial.print(setting.version.minor);
    Serial.print(F("."));
    Serial.println(setting.version.patch);

    Serial.print(F("  - calValue: "));
    Serial.println(setting.calValue);

    Serial.print(F("  - spoolHolderWeight: "));
    Serial.println(setting.spoolHolderWeight);

    Serial.print(F("  - spooderIDSetStatus: 0x"));
    Serial.println(setting.spooderIDSetStatus, HEX);

    Serial.print(F("  - spooderIDLetter: "));
    Serial.println(setting.spooderIDLetter);

    Serial.print(F("  - spooderIDNumber: "));
    Serial.println(setting.spooderIDNumber);

    Serial.print(F("  - lowFilamentThreshold: "));
    Serial.println(setting.lowFilamentThreshold);

    Serial.print(F("  - notifyOnPrintStarted: "));

    Serial.println(setting.notifyOnPrintStarted);

    Serial.print(F("  - notifyOnPrintCompleted: "));
    Serial.println(setting.notifyOnPrintCompleted);

    Serial.print(F("  - notifyOnLowFilament: "));
    Serial.println(setting.notifyOnLowFilament);

    Serial.print(F("  - notifyOnFallOffRack: "));
    Serial.println(setting.notifyOnFallOffRack);

    Serial.print(F("  - notifyOnFallOffBearing: "));
    Serial.println(setting.notifyOnFallOffBearing);

    Serial.print(F("  - notifyOnTangled: "));
    Serial.println(setting.notifyOnTangled);
    Serial.print(F("  - servicesWiFi: "));
    Serial.println(setting.servicesWiFi);
    Serial.print(F("  - servicesMDNS: "));
    Serial.println(setting.servicesMDNS);
    Serial.print(F("  - servicesBLYNK: "));
    Serial.println(setting.servicesBLYNK);
    Serial.print(F("  - servicesWebServer: "));
    Serial.println(setting.servicesWebServer);
    Serial.print(F("  - servicesArduinoOTA: "));
    Serial.println(setting.servicesArduinoOTA);

    Serial.println(F("Setting dump completed."));
}
void FILAMENT_ESTIMATOR::dumpEEPROM()
{
    Serial.println(F("Dump EEPROM data:"));
    for (uint16_t addr = 0; addr < DECLARED_EEPROM_SIZE; addr++)
    {
        if (addr % 64 == 0)
        {
            Serial.println("");
            Serial.print("[");

            if (addr == 0)
                Serial.print("00");
            if (addr == 64)
                Serial.print("0");

            Serial.print(addr);
            Serial.print("]:");
        }

        if (EEPROM.read(addr) < 10)
            Serial.print("0");
        Serial.print(EEPROM.read(addr), HEX);
        Serial.print(" ");
    }
    Serial.println();
}
void FILAMENT_ESTIMATOR::eraseEEPROM()
{
    Serial.println(F("EEPROM data erased."));
    for (uint16_t addr = 0; addr < DECLARED_EEPROM_SIZE; addr++)
    {
        EEPROM.write(addr, 0xff);
    }
    EEPROM.commit();
}
uint32_t FILAMENT_ESTIMATOR::versionToNumber(VERSION_STRUCT v)
{
    //a simple conversion
    //v1.2.3 will convert to something like 10203
    //So that the max number for each item that works will be 99
    uint32_t number = 0;
    number += v.major * 10000;
    number += v.minor * 100;
    number += v.patch * 1;
    return number;
}
void FILAMENT_ESTIMATOR::listDir(const char *dirname)
{
    Serial.printf("Listing directory: %s\r\n", dirname);
    Serial.println("<root>");
    _listDir(dirname, 0);
    FSInfo fs_info;

    LittleFS.info(fs_info);
    Serial.print(F("File system used: "));
    Serial.print(fs_info.usedBytes);
    Serial.println(F(" Bytes"));
    Serial.print(F("File system total: "));
    Serial.print(fs_info.totalBytes);
    Serial.println(F(" Bytes"));
}
void FILAMENT_ESTIMATOR::_listDir(const char *dirname, uint8_t level)
{
    Dir dir = LittleFS.openDir(dirname);
    level++;
    while (dir.next())
    {
        Serial.print(F("|"));
        for (uint8_t i = 0; i < level; i++)
        {
            if (i > 0)
            {
                Serial.print(F("   "));
                if (i == (level - 1))
                {
                    Serial.print(F("|"));
                }
            }
            else //i=0
            {
                //Serial.print("--");
            }
            //Serial.print("--");
        }
        Serial.print("--");
        if (dir.isFile())
        {

            Serial.print(dir.fileName());
            Serial.print(F("  "));
            Serial.println(dir.fileSize());
        }
        else //isDirectory()
        {
            Serial.print("<");
            Serial.print(dir.fileName());
            Serial.println(">");
            _listDir((dirname + String("/") + dir.fileName()).c_str(), level);
        }
    }

    level++;
}
void FILAMENT_ESTIMATOR::displayMonoBitmap(const char *filename)
{
    File f = LittleFS.open(filename, "r");
    Serial.print(filename);
    if (!f)
    {
        Serial.println(F(" - File open failed."));
        f.close();
        return;
    }

    Serial.println(F(" - File open succeeded."));
    Serial.print(F("File size: "));
    Serial.print(f.size());
    Serial.println(F(" Bytes."));

    if (f.size() < 1024)
    {
        Serial.print(F("File size too small(<1024). "));
        return;
    }

    struct
    {
        char signature[2];
        uint32_t filesize = 0;
        uint32_t offset = 0;
        uint32_t width = 0;
        uint32_t height = 0;
        uint32_t colors = 0;
        uint32_t bitmapInfoHeaderSize = 0;

    } bmp;
    bmp.signature[0] = f.read();
    bmp.signature[1] = f.read();
    bool mismatch = false;
    if (!(bmp.signature[0] == 0x42 && bmp.signature[1] == 0x4d))
    {
        Serial.print(F("Not a bitmap file."));
        mismatch = true;
    }
    bmp.filesize = read32(f, 0x03);
    bmp.offset = read32(f, 0x0a);
    bmp.width = read32(f, 0x12);
    bmp.height = read32(f, 0x16);
    bmp.colors = read32(f, 0x2E);
    bmp.bitmapInfoHeaderSize = read32(f, 0x0E);

    if (bmp.width != 128 && bmp.height != 64)
    {
        Serial.println(F("BMP file width and height mismatch."));
        Serial.print(F("Width = "));
        Serial.println(bmp.width);
        Serial.print(F("Height = "));
        Serial.println(bmp.height);
        mismatch = true;
    }
    // Serial.print(F("Header Offset = "));
    // Serial.println(bmp.offset);
    if (bmp.colors != 2)
    {
        Serial.println(F("BMP file color mismatch."));
        Serial.print(F("Colors = "));
        Serial.println(bmp.colors);
        mismatch = true;
    }
    if (mismatch == true)
        return;
    // Serial.print(F("Header size = "));
    // Serial.println(bmp.bitmapInfoHeaderSize);
    uint8_t colorOffset = 14 + bmp.bitmapInfoHeaderSize;
    f.seek(colorOffset);
    bool normalDir;
    if (f.read() == 0 && f.read() == 0 && f.read() == 0)
    {
        //if the color 0 is (0,0,0) black,
        //bits of 0 is black
        normalDir = true;
    }
    else
    {
        normalDir = false;
    }
    //dump the file for debugging
    //f.seek(0);
    // while (f.available())
    // {
    //     char c = f.read();
    //     if (c < 10)
    //         Serial.print("0");
    //     Serial.print(c, HEX);
    //     Serial.print("  ");
    // }
    uint8_t buffer[1024];
    f.seek(bmp.offset);
    for (uint16_t i = 0; i < 1024; i++)
    {
        if (normalDir == true)
        {
            //bits of 0 means black in the bmp
            buffer[1023 - i] = f.read();
        }
        else
        {
            //bits of 0 means white in the bmp, reading is inverted
            buffer[1023 - i] = ~f.read();
        }
    }
    display.drawXbm(0, 0, 128, 64, buffer);
    drawDisplay();
    f.close();
}
uint32_t FILAMENT_ESTIMATOR::read32(File f, uint32_t offset)
{
    f.seek(offset);
    uint32_t r = 0;
    for (uint8_t i = 0; i < 4; i++)
    {
        r += f.read() << (i * 8);
    }
    return r;
}
void FILAMENT_ESTIMATOR::loadConfig()
{
    File configFile = LittleFS.open("/config.json", "r");
    if (!configFile)
    {
        Serial.println(F("Failed to open config file."));
        return;
    }
    configSize = configFile.size();
    if (configSize > JSON_DOC_BUFFER_SIZE)
    {
        Serial.println(F("Config file size is too large"));
        return;
    }

    //Use local json buffer to free up memory after loading
    DynamicJsonDocument jsonDoc(JSON_DOC_BUFFER_SIZE);

    deserializeJson(jsonDoc, configFile);

    strlcpy(config_version, jsonDoc["config_version"] | "", 16);
    strlcpy(wifi_ssid, jsonDoc["wifi_ssid"] | "", 32);
    strlcpy(wifi_password, jsonDoc["wifi_password"] | "", 63);
    strlcpy(blynk_auth, jsonDoc["blynk_auth"] | "", 32);

    uint8_t index = 0;
    for (JsonObject elem : jsonDoc["spool_holder"].as<JsonArray>())
    {
        strlcpy(spoolHolderSlotName[index], elem["name"] | "", 12);
        //spoolHolderSlotName[index] = elem["name"];     // "esun black", "your_holder_name_here", "your_other_holder", ...
        spoolHolderSlotWeight[index] = elem["weight"]; // 180, 150, 250, 250, 250
        index++;
    }
    spoolHolderSlotSize = index;

    Serial.print(F("Config file version "));
    Serial.print(config_version);

    Serial.println(F(" loaded."));
}
void FILAMENT_ESTIMATOR::dumpConfig()
{
    Serial.println(F("Dump config:"));
    //Serial.println(F("Serialized jsonDoc content:"));
    //serializeJson(jsonDoc, Serial);
    Serial.println();
    Serial.println(F("Deserialized data:"));
    Serial.print(F("config_version:"));
    Serial.println(config_version);
    Serial.print(F("wifi_ssid:"));
    Serial.println(wifi_ssid);
    Serial.print(F("wifi_password:"));
    Serial.println(wifi_password);
    Serial.print(F("blynk_auth:"));
    Serial.println(blynk_auth);

    for (uint8_t index = 0; index < spoolHolderSlotSize; index++)
    {
        Serial.print(F("name:"));
        Serial.print(spoolHolderSlotName[index]);
        Serial.print(F("  weight:"));
        Serial.println(spoolHolderSlotWeight[index]);
        index++;
    }
    Serial.print(F("spoolHolderSlotSize = "));
    Serial.println(spoolHolderSlotSize);
    Serial.println(F("Dump config completed."));
}
void FILAMENT_ESTIMATOR::replyOK()
{
    webServer->send(200, FPSTR(TEXT_PLAIN), "");
}
void FILAMENT_ESTIMATOR::replyOKWithMsg(String msg)
{
    webServer->send(200, FPSTR(TEXT_PLAIN), msg);
}
void FILAMENT_ESTIMATOR::replyNotFound(String msg)
{
    webServer->send(404, FPSTR(TEXT_PLAIN), msg);
}
void FILAMENT_ESTIMATOR::replyBadRequest(String msg)
{
    Serial.println(msg);
    webServer->send(400, FPSTR(TEXT_PLAIN), msg + "\r\n");
}
void FILAMENT_ESTIMATOR::replyServerError(String msg)
{
    Serial.println(msg);
    webServer->send(500, FPSTR(TEXT_PLAIN), msg + "\r\n");
}
void FILAMENT_ESTIMATOR::handleStatus()
{
    Serial.println(F("handleStatus"));
    FSInfo fs_info;
    String json;
    json.reserve(128);

    json = "{\"type\":\"";
    json += fsName;
    json += "\", \"isOk\":";
    if (fsOK)
    {
        fileSystem->info(fs_info);
        json += F("\"true\", \"totalBytes\":\"");
        json += fs_info.totalBytes;
        json += F("\", \"usedBytes\":\"");
        json += fs_info.usedBytes;
        json += "\"";
    }
    else
    {
        json += "\"false\"";
    }
    json += F(",\"unsupportedFiles\":\"");
    json += unsupportedFiles;
    json += "\"}";

    webServer->send(200, "application/json", json);
}
void FILAMENT_ESTIMATOR::handleFileList()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }

    if (!webServer->hasArg("dir"))
    {
        return replyBadRequest(F("DIR ARG MISSING"));
    }

    String path = webServer->arg("dir");
    if (path != "/" && !fileSystem->exists(path))
    {
        return replyBadRequest(F("BAD PATH"));
    }

    Serial.println(String("handleFileList: ") + path);
    Dir dir = fileSystem->openDir(path);
    path.clear();

    // use HTTP/1.1 Chunked response to avoid building a huge temporary string
    if (!webServer->chunkedResponseModeStart(200, "text/json"))
    {
        webServer->send(505, F("text/html"), F("HTTP1.1 required"));
        return;
    }

    // use the same string for every line
    String output;
    output.reserve(64);
    while (dir.next())
    {

        if (output.length())
        {
            // send string from previous iteration
            // as an HTTP chunk
            webServer->sendContent(output);
            output = ',';
        }
        else
        {
            output = '[';
        }

        output += "{\"type\":\"";
        if (dir.isDirectory())
        {
            output += "dir";
        }
        else
        {
            output += F("file\",\"size\":\"");
            output += dir.fileSize();
        }

        output += F("\",\"name\":\"");
        // Always return names without leading "/"
        if (dir.fileName()[0] == '/')
        {
            output += &(dir.fileName()[1]);
        }
        else
        {
            output += dir.fileName();
        }

        output += "\"}";
    }

    // send last string
    output += "]";
    webServer->sendContent(output);
    webServer->chunkedResponseFinalize();
}
bool FILAMENT_ESTIMATOR::handleFileRead(String path)
{
    Serial.println(String("handleFileRead: ") + path);
    if (!fsOK)
    {
        replyServerError(FPSTR(FS_INIT_ERROR));
        return true;
    }

    if (path.endsWith("/"))
    {
        path += "index.htm";
    }

    String contentType;
    if (webServer->hasArg("download"))
    {
        contentType = F("application/octet-stream");
    }
    else
    {
        contentType = mime::getContentType(path);
    }

    if (!fileSystem->exists(path))
    {
        // File not found, try gzip version
        path = path + ".gz";
    }
    if (fileSystem->exists(path))
    {
        File file = fileSystem->open(path, "r");
        if (webServer->streamFile(file, contentType) != file.size())
        {
            Serial.println(F("Sent less data than expected!"));
        }
        file.close();
        return true;
    }

    return false;
}
String FILAMENT_ESTIMATOR::lastExistingParent(String path)
{
    while (!path.isEmpty() && !fileSystem->exists(path))
    {
        if (path.lastIndexOf('/') > 0)
        {
            path = path.substring(0, path.lastIndexOf('/'));
        }
        else
        {
            path = String(); // No slash => the top folder does not exist
        }
    }
    Serial.println(String("Last existing parent: ") + path);
    return path;
}
void FILAMENT_ESTIMATOR::handleFileCreate()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }

    String path = webServer->arg("path");
    if (path.isEmpty())
    {
        return replyBadRequest(F("PATH ARG MISSING"));
    }

    if (path == "/")
    {
        return replyBadRequest(F("BAD PATH"));
    }
    if (fileSystem->exists(path))
    {
        return replyBadRequest(F("PATH FILE EXISTS"));
    }

    String src = webServer->arg("src");
    if (src.isEmpty())
    {
        // No source specified: creation
        Serial.println(String("handleFileCreate: ") + path);
        if (path.endsWith("/"))
        {
            // Create a folder
            path.remove(path.length() - 1);
            if (!fileSystem->mkdir(path))
            {
                return replyServerError(F("MKDIR FAILED"));
            }
        }
        else
        {
            // Create a file
            File file = fileSystem->open(path, "w");
            if (file)
            {
                file.write((const char *)0);
                file.close();
            }
            else
            {
                return replyServerError(F("CREATE FAILED"));
            }
        }
        if (path.lastIndexOf('/') > -1)
        {
            path = path.substring(0, path.lastIndexOf('/'));
        }
        replyOKWithMsg(path);
    }
    else
    {
        // Source specified: rename
        if (src == "/")
        {
            return replyBadRequest("BAD SRC");
        }
        if (!fileSystem->exists(src))
        {
            return replyBadRequest(F("SRC FILE NOT FOUND"));
        }

        Serial.println(String("handleFileCreate: ") + path + " from " + src);

        if (path.endsWith("/"))
        {
            path.remove(path.length() - 1);
        }
        if (src.endsWith("/"))
        {
            src.remove(src.length() - 1);
        }
        if (!fileSystem->rename(src, path))
        {
            return replyServerError(F("RENAME FAILED"));
        }
        replyOKWithMsg(lastExistingParent(src));
    }
}
void FILAMENT_ESTIMATOR::deleteRecursive(String path)
{
    File file = fileSystem->open(path, "r");
    bool isDir = file.isDirectory();
    file.close();

    // If it's a plain file, delete it
    if (!isDir)
    {
        fileSystem->remove(path);
        return;
    }

    // Otherwise delete its contents first
    Dir dir = fileSystem->openDir(path);

    while (dir.next())
    {
        deleteRecursive(path + '/' + dir.fileName());
    }

    // Then delete the folder itself
    fileSystem->rmdir(path);
}
void FILAMENT_ESTIMATOR::handleFileDelete()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }

    String path = webServer->arg(0);
    if (path.isEmpty() || path == "/")
    {
        return replyBadRequest("BAD PATH");
    }

    Serial.println(String("handleFileDelete: ") + path);
    if (!fileSystem->exists(path))
    {
        return replyNotFound(FPSTR(FILE_NOT_FOUND));
    }
    deleteRecursive(path);

    replyOKWithMsg(lastExistingParent(path));
}
void FILAMENT_ESTIMATOR::handleFileUpload()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }
    if (webServer->uri() != "/edit")
    {
        return;
    }
    HTTPUpload &upload = webServer->upload();
    if (upload.status == UPLOAD_FILE_START)
    {
        String filename = upload.filename;
        // Make sure paths always start with "/"
        if (!filename.startsWith("/"))
        {
            filename = "/" + filename;
        }
        Serial.println(String("handleFileUpload Name: ") + filename);
        uploadFile = fileSystem->open(filename, "w");
        if (!uploadFile)
        {
            return replyServerError(F("CREATE FAILED"));
        }
        Serial.println(String("Upload: START, filename: ") + filename);
    }
    else if (upload.status == UPLOAD_FILE_WRITE)
    {
        if (uploadFile)
        {
            size_t bytesWritten = uploadFile.write(upload.buf, upload.currentSize);
            if (bytesWritten != upload.currentSize)
            {
                return replyServerError(F("WRITE FAILED"));
            }
        }
        Serial.println(String("Upload: WRITE, Bytes: ") + upload.currentSize);
    }
    else if (upload.status == UPLOAD_FILE_END)
    {
        if (uploadFile)
        {
            uploadFile.close();
        }
        Serial.println(String("Upload: END, Size: ") + upload.totalSize);
    }
}
void FILAMENT_ESTIMATOR::handleNotFound()
{
    if (!fsOK)
    {
        return replyServerError(FPSTR(FS_INIT_ERROR));
    }

    String uri = ESP8266WebServer::urlDecode(webServer->uri()); // required to read paths with blanks

    if (handleFileRead(uri))
    {
        return;
    }

    // Dump debug data
    String message;
    message.reserve(100);
    message = F("Error: File not found\n\nURI: ");
    message += uri;
    message += F("\nMethod: ");
    message += (webServer->method() == HTTP_GET) ? "GET" : "POST";
    message += F("\nArguments: ");
    message += webServer->args();
    message += '\n';
    for (uint8_t i = 0; i < webServer->args(); i++)
    {
        message += F(" NAME:");
        message += webServer->argName(i);
        message += F("\n VALUE:");
        message += webServer->arg(i);
        message += '\n';
    }
    message += "path=";
    message += webServer->arg("path");
    message += '\n';
    Serial.print(message);

    return replyNotFound(message);
}
void FILAMENT_ESTIMATOR::handleGetEdit()
{
    if (handleFileRead(F("/edit/index.htm")))
    {
        return;
    }
    replyNotFound(FPSTR(FILE_NOT_FOUND));
}
void FILAMENT_ESTIMATOR::startLogging()
{
    Serial.println(F("Start logging."));
    String filename = "/log/log";
    uint8_t suf = 1;
    filename += suf;
    filename += ".txt";

    while (LittleFS.exists(filename))
    {
        filename = "/log/log";
        suf += 1;
        filename += suf;
        filename += ".txt";
        yield();
    }
    Serial.print(F("Logging into file: "));
    Serial.println(filename);
    logFile = LittleFS.open(filename, "a");
    isLogging = true;
}
void FILAMENT_ESTIMATOR::stopLogging()
{
    Serial.println(F("Stop logging."));
    logFile.close();
    isLogging = false;
}
void FILAMENT_ESTIMATOR::updateLogging()
{
    //logs every second
    now = time(nullptr);
    if (now != previous)
    {
        String record = (String)now;
        record += ",";
        record += (String)totalWeight;
        if (logFile.println(record))
        {
            Serial.print("Logging: ");
            Serial.println(record);
        }
        previous = now;
    }
}
void FILAMENT_ESTIMATOR::startEmulation()
{
    emulatedLogFile = LittleFS.open("log/log.txt", "r");
    Serial.print("log/log.txt");
    if (!emulatedLogFile)
    {
        Serial.println(F(" - File open failed."));
        emulatedLogFile.close();
        return;
    }
    drawOverlay("Start", "Emulation", 1000);
    Serial.println(F(" - File open succeeded."));
    Serial.print(F("File size: "));
    Serial.print(emulatedLogFile.size());
    Serial.println(F(" Bytes."));

    emulationStarted = true;
    Serial.println(F("Emulation started."));
    EMULATION_PERIOD = 100;
    DETECTION_PERIOD = 100;
    loggerCounter = 0;
    detectionTimer = millis(); //reset timer, to fill the weight first, debug purpose
}
void FILAMENT_ESTIMATOR::stopEmulation()
{
    if (emulatedLogFile == true)
    {
        drawOverlay("Stop", "Emulation", 1000);
        Serial.println(F("Emulation stopped."));
        emulatedLogFile.close();
        emulationStarted = false;
        EMULATION_PERIOD = 1000;
        DETECTION_PERIOD = 1000;
    }
}
void FILAMENT_ESTIMATOR::updateEmulation()
{
    if (millis() - emulationTimer < EMULATION_PERIOD)
        return;
    if (emulatedLogFile.available() > 5) //normally a valid data set has more than 4 bytes
    {
        //Reading a set of data
        emulatedLogFile.parseInt();
        emulatedLogFile.read();
        emulatedWeight = emulatedLogFile.parseFloat();
    }
    else
    {
        stopEmulation();
    }

    emulationTimer = millis();
}
void FILAMENT_ESTIMATOR::updateDetection()
{
    if (millis() - detectionTimer < DETECTION_PERIOD)
        return;

    float change = totalWeight - previousTotalWeight;
    float mean = getMean(weightCount);
    float mean3 = getMean(3);
    float stddev = getStddev(weightCount);
    float stddev3 = getStddev(3);

    pushWeight(totalWeight);
    pushStddev(stddev3);
    //calculate how many samples has the stddev3>1 condition, in the last 30 seconds
    uint8_t stddev3Count = getStddevCount(1.0);

    time_t now = time(nullptr);
    tm *tmp = localtime(&now);
    String timeStamp;
    if (tmp->tm_hour < 10)
        timeStamp += "0";
    timeStamp += tmp->tm_hour;
    timeStamp += ":";
    if (tmp->tm_min < 10)
        timeStamp += "0";
    timeStamp += tmp->tm_min;
    timeStamp += ":";
    if (tmp->tm_sec < 10)
        timeStamp += "0";
    timeStamp += tmp->tm_sec;

    if (detectionDebugOutput == true)
    {
        Serial.print(timeStamp);
        Serial.print(" Count = ");
        Serial.print(weightCount);
        Serial.print(" Total = ");
        if (totalWeight >= 0)
            Serial.print(F(" "));
        Serial.print(totalWeight);
        Serial.print("  Change = ");
        if (change >= 0)
            Serial.print(F(" "));
        Serial.print(change);
        Serial.print("  Mean = ");
        if (mean >= 0)
            Serial.print(F(" "));
        Serial.print(mean);
        Serial.print("  Mean(3) = ");
        if (mean3 >= 0)
            Serial.print(F(" "));
        Serial.print(mean3);
        Serial.print("  Stddev = ");
        if (stddev >= 0)
            Serial.print(F(" "));
        Serial.print(stddev);
        Serial.print("  Stddev(3) = ");
        if (stddev3 >= 0)
            Serial.print(F(" "));
        Serial.print(stddev3);

        Serial.print("  stddev3Count = ");
        if (stddev3Count < 10)
            Serial.print(F(" "));
        Serial.print(stddev3Count);

        Serial.print("  printingStatus = ");
        Serial.print(printingStatusString);
        Serial.println();
    }

    if (isLogging == true)
    {
        //logs every second

        String record;
        record += (String)timeStamp;
        record += ",";
        record += (String)loggerCounter;
        record += ",";
        record += (String)totalWeight;
        record += ",";
        record += (String)change;
        record += ",";
        record += (String)stddev;
        record += ",";
        record += (String)stddev3;
        record += ",";
        record += (String)stddev3Count;
        record += ",";
        record += (String)printingStatusString;
        if (logFile.println(record))
        {
            //Serial.print("Logging: ");
            //Serial.println(record);
        }
        loggerCounter++;
    }

    switch (printingStatus)
    {
    case STATUS_BOOT:
        if (weightCount > 2 && getMean(3) < 30 && getStddev(3) < 0.5)
        {
            printingStatus = STATUS_EMPTY;
            printingStatusString = "STATUS_EMPTY";
            Serial.println(F("Status changed from STATUS_BOOT to STATUS_EMPTY."));
            purgeCounter = 3; //purge array
            if (isLogging == true)
            {
                String record;
                record += (String)loggerCounter;
                record += ",";
                record += "Status changed from STATUS_BOOT to STATUS_EMPTY.";
                logFile.println(record);
            }
        }
        break;
    case STATUS_EMPTY:
        if (getMean(3) >= (setting.spoolHolderWeight - 50) && getStddev(3) < 0.5)
        {
            if (purgeCounter == 0)
            {
                //Do not change state during purge counting
                printingStatus = STATUS_IDLE;
                printingStatusString = "STATUS_IDLE";
                Serial.println(F("Status changed from STATUS_EMPTY to STATUS_IDLE."));
                purgeCounter = 3; //purge array after few samples
                if (isLogging == true)
                {
                    String record;
                    record += (String)loggerCounter;
                    record += ",";
                    record += "Status changed from STATUS_EMPTY to STATUS_IDLE.";
                    logFile.println(record);
                }
            }
        }
        break;
    case STATUS_IDLE:
        if (getMean(3) < (setting.spoolHolderWeight - 50) && getStddev(3) < 0.5)
        {
            if (purgeCounter == 0)
            {
                //Do not change state during purge counting
                printingStatus = STATUS_EMPTY;
                printingStatusString = "STATUS_EMPTY";
                Serial.println(F("Status changed from STATUS_IDLE to STATUS_EMPTY."));
                purgeCounter = 3; //purge array after few samples
                if (isLogging == true)
                {
                    String record;
                    record += (String)loggerCounter;
                    record += ",";
                    record += "Status changed from STATUS_IDLE to STATUS_EMPTY.";
                    logFile.println(record);
                }
            }
        }
        else if (getStddev(30) >= 1 && getStddev(30) < 30 && stddev3Count >= 10)
        {
            //Print started
            //From STATUS_IDLE to STATUS_PRINTING
            //(1) 30 sec stddev between 1 and 30
            //(2) 10 of the last 30 stddev(3) samples is more than 1.0

            //Reset all notification flags
            lowFilamentNotificationSent = false;
            fallOffRackNotificationSent = false;

            printingStatus = STATUS_PRINTING;
            printingStatusString = "STATUS_PRINTING";
            Serial.println(F("Status changed from STATUS_IDLE to STATUS_PRINTING."));
            notify(NOTIFICATION_PRINT_STARTED);
            if (isLogging == true)
            {
                String record;
                record += (String)loggerCounter;
                record += ",";
                record += "Status changed from STATUS_IDLE to STATUS_PRINTING.";
                record += ",";
                record += "NOTIFICATION_PRINT_STARTED sent";
                logFile.println(record);
            }
        }

        break;
    case STATUS_PRINTING:
        if (getStddev(30) < 0.5 && stddev3Count < 3)
        {
            //print job completed
            printingStatus = STATUS_IDLE;
            printingStatusString = "STATUS_IDLE";
            Serial.println(F("Status changed from STATUS_PRINTING to STATUS_IDLE."));
            notify(NOTIFICATION_PRINT_COMPLETED);

            if (isLogging == true)
            {
                String record;
                record += (String)loggerCounter;
                record += ",";
                record += "Status changed from STATUS_PRINTING to STATUS_IDLE.";
                record += ",";
                record += "NOTIFICATION_PRINT_COMPLETED sent";
                logFile.println(record);
            }
        }

        if (fallOffRackNotificationSent == false)
        {
            if (getStddev(10) > 100 && getMean(3) < (setting.spoolHolderWeight - 50))
            {
                //fall of rack, once per print, reset flag upon new print
                notify(NOTIFICATIONI_FALL_OFF_RACK);
                fallOffRackNotificationSent = true;

                lowFilamentNotificationSent = true; //inhibit low filament notification
            }
        }

        if (lowFilamentNotificationSent == false)
        {
            if (filamentWeight < setting.lowFilamentThreshold && getStddev(30) < 30)
            {
                //notification sent once per print, reset flag upon new print
                notify(NOTIFICATIONI_LOW_FILAMENT);
                lowFilamentNotificationSent = true;
            }
        }
        break;
    default:
        break;
    } //switch (printingStatus)

    if (purgeCounter > 0)
    {
        purgeCounter--;
        if (purgeCounter == 0)
        {
            purgeWeight(totalWeight);
            purgeStddev(0);
        }
    }

    previousTotalWeight = totalWeight;

    //globalPrintMemory(); //debug oom

    detectionTimer = millis();
}
void FILAMENT_ESTIMATOR::pushWeight(float entry)
{
    if (weightCount < DETECTION_SAMPLE_SIZE)
    {
        weightCount++;
    }
    weightArray[detectionPosition] = entry;
    detectionPosition += 1;
    if (detectionPosition == DETECTION_SAMPLE_SIZE)
    {
        detectionPosition = 0;
    }
}
void FILAMENT_ESTIMATOR::pushStddev(float entry)
{
    if (stddevCount < DETECTION_SAMPLE_SIZE)
    {
        stddevCount++;
    }
    stddevArray[stddevPosition] = entry;
    stddevPosition += 1;
    if (stddevPosition == DETECTION_SAMPLE_SIZE)
    {
        stddevPosition = 0;
    }
}
void FILAMENT_ESTIMATOR::purgeWeight(float value)
{
    //purge the whole detectionArray with single value
    //used when there is a condition change
    for (uint16_t i = 0; i < DETECTION_SAMPLE_SIZE; i++)
    {
        pushWeight(value);
    }
    Serial.println(F("Weight Array purged."));
}
void FILAMENT_ESTIMATOR::purgeStddev(float value)
{
    //purge the whole detectionArray with single value
    //used when there is a condition change
    for (uint16_t i = 0; i < DETECTION_SAMPLE_SIZE; i++)
    {
        pushStddev(value);
    }
    Serial.println(F("Stddev Array purged."));
}
float FILAMENT_ESTIMATOR::getSum(uint16_t samples)
{
    if (weightCount == 0 || samples == 0)
    {
        return 0;
    }
    else
    {
        if (samples > weightCount)
        {
            samples = weightCount;
        }
        if (samples > DETECTION_SAMPLE_SIZE)
        {
            samples = DETECTION_SAMPLE_SIZE;
        }
    }
    float sum = 0;
    int16_t getArrayPosition;
    for (uint8_t i = 0; i < samples; i++)
    {
        getArrayPosition = detectionPosition - 1 - i;
        if (getArrayPosition < 0)
        {
            getArrayPosition += DETECTION_SAMPLE_SIZE;
        }
        sum += weightArray[getArrayPosition];
    }
    return sum;
}
float FILAMENT_ESTIMATOR::getMean(uint16_t samples)
{
    if (weightCount == 0 || samples == 0)
    {
        return 0;
    }
    float sum = getSum(samples);
    return sum / samples;
}
float FILAMENT_ESTIMATOR::getStddev(uint16_t samples)
{
    if (weightCount == 0 || samples == 0)
    {
        return 0;
    }
    else
    {
        if (samples > weightCount)
        {
            samples = weightCount;
        }
        if (samples > DETECTION_SAMPLE_SIZE)
        {
            samples = DETECTION_SAMPLE_SIZE;
        }
    }
    float sum = 0;
    float square;
    float mu = getMean(samples);
    float theta;
    int16_t getArrayPosition;
    for (uint16_t i = 0; i < samples; i++)
    {
        getArrayPosition = detectionPosition - 1 - i;
        if (getArrayPosition < 0)
        {
            getArrayPosition += DETECTION_SAMPLE_SIZE;
        }
        theta = mu - weightArray[getArrayPosition];
        square = theta * theta;
        sum += square;
    }
    return sqrtf(sum / samples);
}
uint8_t FILAMENT_ESTIMATOR::getStddevCount(float threshold)
{
    uint8_t count = 0;
    for (uint8_t i = 0; i < DETECTION_SAMPLE_SIZE; i++)
    {
        if (stddevArray[i] > threshold)
        {
            count++;
        }
    }
    return count;
}
void FILAMENT_ESTIMATOR::notify(NOTIFICATION_MESSAGE message)
{
    if (!Blynk.connected())
    {
        Serial.println(F("Failed. Blynk not connected."));
        return;
    }
    switch (message)
    {
    case NOTIFICATION_TEST_MESSAGE:
        Blynk.notify(hostname + " test notification message");
        drawOverlay("Test msg", "Sent", 1000);
        Serial.println(F("Blynk notification message sent."));
        break;
    case NOTIFICATION_PRINT_STARTED:
        Serial.println(F("NOTIFICATION_PRINT_STARTED"));
        if (setting.notifyOnPrintStarted == true)
        {
            Blynk.notify(hostname + " print job started.");
            Serial.println(F("Blynk NOTIFICATION_PRINT_STARTED sent."));
        }
        drawOverlay("Print job", "Started", 1000);

        break;
    case NOTIFICATION_PRINT_COMPLETED:
        Serial.println(F("NOTIFICATION_PRINT_COMPLETED"));
        if (setting.notifyOnPrintCompleted == true)
        {
            Blynk.notify(hostname + " print job completed.");
            Serial.println(F("Blynk NOTIFICATION_PRINT_COMPLETED sent."));
        }
        drawOverlay("Print job", "Completed", 1000);
        break;
    case NOTIFICATIONI_LOW_FILAMENT:
        Serial.println(F("NOTIFICATIONI_LOW_FILAMENT"));
        if (setting.notifyOnLowFilament == true)
        {
            Blynk.notify(hostname + " low filament(" + setting.lowFilamentThreshold + "g).");
            Serial.println(F("Blynk NOTIFICATIONI_LOW_FILAMENT sent."));
        }
        drawOverlay("Low", "Filament", 1000);
        break;
    case NOTIFICATIONI_FALL_OFF_RACK:
        Serial.println(F("NOTIFICATIONI_FALL_OFF_RACK"));
        if (setting.notifyOnFallOffRack == true)
        {
            Blynk.notify(hostname + " fall off rack.");
            Serial.println(F("Blynk NOTIFICATIONI_FALL_OFF_RACK sent."));
        }
        drawOverlay("Fall Off", "Rack", 1000);
        break;
    case NOTIFICATIONI_FALL_OFF_BEARING:
        Serial.println(F("NOTIFICATIONI_FALL_OFF_BEARING"));
        if (setting.notifyOnFallOffBearing == true)
        {
            Blynk.notify(hostname + " fall off bearing.");
            Serial.println(F("Blynk NOTIFICATIONI_FALL_OFF_BEARING sent."));
        }
        drawOverlay("Fall Off", "Bearing", 1000);
        break;
    case NOTIFICATIONI_TANGLED:
        Serial.println(F("NOTIFICATIONI_TANGLED"));
        if (setting.notifyOnTangled == true)
        {
            Blynk.notify(hostname + " tangled.");
            Serial.println(F("Blynk NOTIFICATIONI_TANGLED sent."));
        }
        drawOverlay("Filament", "Tangled", 1000);
        break;
    default:
        break;
    }
}
bool FILAMENT_ESTIMATOR::getNotificationSetting(uint8_t selection)
{
    bool result = false;
    switch (selection)
    {
    case NOTIFICATION_MENU_PRINT_STARTED:
        result = setting.notifyOnPrintStarted;
        break;
    case NOTIFICATION_MENU_PRINT_COMPLETED:
        result = setting.notifyOnPrintCompleted;
        break;
    case NOTIFICATION_MENU_LOW_FILAMENTT:
        result = setting.notifyOnLowFilament;
        break;
    case NOTIFICATION_MENU_FALL_OFF_RACK:
        result = setting.notifyOnFallOffRack;
        break;
    case NOTIFICATION_MENU_FALL_OFF_BEARING:
        result = setting.notifyOnFallOffBearing;
        break;
    case NOTIFICATION_MENU_TANGLED:
        result = setting.notifyOnTangled;
        break;
    default:
        break;
    }
    return result;
}
void FILAMENT_ESTIMATOR::setNotificationSetting(uint8_t selection, bool value)
{
    switch (selection)
    {
    case NOTIFICATION_MENU_PRINT_STARTED:
        setting.notifyOnPrintStarted = value;
        break;
    case NOTIFICATION_MENU_PRINT_COMPLETED:
        setting.notifyOnPrintCompleted = value;
        break;
    case NOTIFICATION_MENU_LOW_FILAMENTT:
        setting.notifyOnLowFilament = value;
        break;
    case NOTIFICATION_MENU_FALL_OFF_RACK:
        setting.notifyOnFallOffRack = value;
        break;
    case NOTIFICATION_MENU_FALL_OFF_BEARING:
        setting.notifyOnFallOffBearing = value;
        break;
    case NOTIFICATION_MENU_TANGLED:
        setting.notifyOnTangled = value;
        break;
    default:
        break;
    }
    saveToEEPROM();
}
void FILAMENT_ESTIMATOR::queryMDNS()
{
    if (MDNS.isRunning() == true)
    {
        Serial.println(F("Query 'spooder' services in the local network:"));
        int n = MDNS.queryService("spooder", "tcp"); // Send out query for esp tcp services
        Serial.println(F("mDNS query done."));
        if (n == 0)
        {
            Serial.println("No services found.");
        }
        else //services found
        {
            Serial.print(n);
            Serial.println(" service(s) found");
            for (int i = 0; i < n; ++i)
            {
                // Print details for each service found
                Serial.print(i + 1);
                Serial.print(": ");
                Serial.print(MDNS.answerHostname(i));
                Serial.print(" (");
                Serial.print(MDNS.answerIP(i));
                Serial.print(":");
                Serial.print(MDNS.answerPort(i));
                //Serial.print(") TXT:");
                //Serial.print(MDNS.answerTxts())

                Serial.println();
            }
        }
    }
    else //MDNS is not running on this device
    {
        Serial.println(F("Failed. mDNS is not running."));
    }
    return;
}
void FILAMENT_ESTIMATOR::updateServiceTxt()
{
    if (MDNS.isRunning() == false)
    {
        Serial.println(F("Failed. mDNS is not running."));
        return;
    }

    Serial.print(F("Update mDNS service txt: filament,"));
    Serial.print((int16_t)filamentWeight);
    Serial.println();

    MDNS.addServiceTxt(spooderService, "filament", (int16_t)filamentWeight);
    // 'announce' can be called every time, the configuration of some service
    // changes. Mainly, this would be changed content of TXT items.
    MDNS.announce();
    return;
}
void FILAMENT_ESTIMATOR::printSpoodersDataset()
{
    if (MDNS.isRunning() == false)
    {
        Serial.println(F("Failed. mDNS is not running."));
        return;
    }
    globalPrintSpoodersDataset();
    return;
}
void FILAMENT_ESTIMATOR::checkGithubTag()
{
    Serial.println(F("Checking github firmware version:"));

    if (WiFi.status() != WL_CONNECTED)
    {
        Serial.println(F("checkGithubTag failed. WiFi not connected."));
        return;
    }
    if (LittleFS.begin())
    {
        Serial.println("LittleFS ok.");
        //listDir("");
    }
    else
    {
        Serial.println("LittleFS failed.");
        return;
    }
    BearSSL::CertStore certStore;
    _certStore = &certStore;
    numCerts = certStore.initCertStore(LittleFS, PSTR("/certs.idx"), PSTR("/certs.ar"));
    Serial.print(F("Number of CA certs read: "));
    Serial.println(numCerts);
    if (numCerts == 0)
    {
        Serial.println(F("No CA certs found. Unable to establish https connection."));
        return;
    }
    globalPrintMemory("After certs read");
    BearSSL::WiFiClientSecure client; //7k memory required
    globalPrintMemory(F("After client creation."));
    client.setCertStore(_certStore);

    if (!client.connect(_host, _port)) //22k memory required
    {
        Serial.println(F("Connection to gitgub failed"));
    }
    else
    {
    }
    globalPrintMemory("After client connection");

    /*String url = "/repos/";
    url += _user;
    url += "/";
    url += _repo;
    url += "/releases/latest";
    */
    //String url = "/repos/FuzzyNoodle/Fuzzy-Spooder/releases/latest";
    String url = "/repos/FuzzyNoodle/Fuzzy-Spooder/tags";
    //String url = "/repos/yknivag/ESP_OTA_GitHub_Showcase/releases/latest";
    //String url = "/repos/yknivag/ESP_OTA_GitHub_Showcase/tags";
    Serial.print(F("Connection to: "));
    Serial.println(url);

    //rate limit to 60 calls per hour

    client.print(String("GET ") + url + " HTTP/1.1\r\n" +
                 "Host: " + _host + "\r\n" +
                 "User-Agent: ESP_OTA_GitHubArduinoLibrary\r\n" +
                 "Connection: close\r\n\r\n");

    while (client.connected())
    {
        String s = client.readString();
        Serial.println(s);
    }

    /*
    while (client.connected())
    {
        String response = client.readStringUntil('\n');
        Serial.println(response);
        if (response == "\r")
        {
            break;
        }
    }
    String response = client.readStringUntil('\n');
    Serial.println(response);
    */

    globalPrintMemory("After printing last response");
}
bool FILAMENT_ESTIMATOR::getServicesSetting(uint8_t selection)
{
    bool result = false;
    switch (selection)
    {
    case SERVICES_MENU_WIFI:
        result = setting.servicesWiFi;
        break;
    case SERVICES_MENU_MDNS:
        result = setting.servicesMDNS;
        break;
    case SERVICES_MENU_BLYNK:
        result = setting.servicesBLYNK;
        break;
    case SERVICES_MENU_WEB_SERVER:
        result = setting.servicesWebServer;
        break;
    case SERVICES_MENU_ARDUINO_OTA:
        result = setting.servicesArduinoOTA;
        break;

    default:
        break;
    }
    return result;
}
void FILAMENT_ESTIMATOR::setServicesSetting(uint8_t selection, bool value)
{
    switch (selection)
    {
    case SERVICES_MENU_WIFI:
        setting.servicesWiFi = value;
        break;
    case SERVICES_MENU_MDNS:
        setting.servicesMDNS = value;
        break;
    case SERVICES_MENU_BLYNK:
        setting.servicesBLYNK = value;
        break;
    case SERVICES_MENU_WEB_SERVER:
        setting.servicesWebServer = value;
        break;
    case SERVICES_MENU_ARDUINO_OTA:
        setting.servicesArduinoOTA = value;
        break;
    default:
        break;
    }
    Serial.print(servicesMenuTitle[selection]);
    Serial.print(F(" set to: "));
    Serial.println((value == true) ? "On" : "Off");
    saveToEEPROM();
}
