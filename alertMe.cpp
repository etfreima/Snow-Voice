#include "pch.h"
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <istream>
#include <unordered_map>
//#include <map>
#include "libs/rapidjson/document.h"
#include "libs/rapidjson/stringbuffer.h"
#include "libs/rapidjson/writer.h"
#include "libs/rapidjson/filereadstream.h"
#include "libs/vcpkg/buildtrees/cpprestsdk/src/v2.10.10-dc225cdbd9/Release/include/cpprest/http_client.h"
#include "C:/Users/anon/curl/builds/libcurl-vc-x64-release-dll-ipv6-sspi-winssl/include/curl/curl.h"
#include "C:/Users/anon/curl/builds/libcurl-vc-x64-release-dll-ipv6-sspi-winssl/include/curl/easy.h"

#pragma comment (lib, "C:/Users/anon/curl/builds/libcurl-vc-x64-release-dll-ipv6-sspi-winssl/lib/libcurl.lib")

#define CURL_STATICLIB

#pragma warning(disable:4996)

//global vars
std::string INP_COORDS;
std::string POSTAL_CODE;
std::string GEO_PATH = "debugOld/repoForecast.txt";

//Pre declaration of functions, are defined under main function
std::string getCoords();
std::string writeLink(std::string fullLink);
std::string getFile(std::string path);
void delLink();

int main()
{
	//Important variable declarations
	std::string preDarkURL = "https://api.darksky.net/forecast/";
	std::string usrCoords;
	std::string line_;
	std::string fullDarkURL;
	std::string fullGeoURL;
	std::string darkPath = "debugOld/infLog.txt";
	
	usrCoords = getCoords();

	//Generates links with user coordinates
	fullDarkURL = preDarkURL + getFile(darkPath) + "/" + INP_COORDS;
	char *charDarkUrl = &fullDarkURL[0u];
	char *charPreDarkUrl = &preDarkURL[0u];

	//Writes the link to a file
	std::cout << writeLink(fullDarkURL);

	//New line
	std::cout << "\n";

	std::string preGeoURL = "https://maps.googleapis.com/maps/api/geocode/json?latlng=";
	fullGeoURL = preGeoURL + INP_COORDS + "&key=" + getFile(GEO_PATH);
	std::cout << fullGeoURL;

	std::cout << "\n";



			//********************************************
			//********************cURL********************

			//Outputs URL contents into a file
			CURL *curl;
			FILE *fd;
			CURLcode res;
			char outfilename[FILENAME_MAX] = "debugOld/repoOld.json";
			curl = curl_easy_init();
			if (curl) {
				fd = fopen(outfilename, "wb");
				curl_easy_setopt(curl, CURLOPT_URL, charDarkUrl);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, fd);
				res = curl_easy_perform(curl);
				curl_easy_cleanup(curl);
				fclose(fd);
			}

			//********************cURL********************
			//********************************************

			//********************************************
			//****************rapidjson*******************
			//thanks to: https://github.com/Tencent/rapidjson/issues/1343
			
			//std::ifstream jFile("../alertMe/debugOld/repoOld.json");
			FILE* fp = fopen("../alertMe/debugOld/repoOld.json", "rb");
			char readBuffer[65536];//in bytes

			rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
			rapidjson::Document d;
			d.ParseStream(is);
			
			const rapidjson::Value& a = d["hourly"];
			const rapidjson::Value& prog = a["data"];

			//Loops through each element of data array
			for (rapidjson::Value::ConstValueIterator p = prog.Begin(); p != prog.End(); p++) {
				std::time_t unixTime = (*p)["time"].GetInt();
				std::string weathSummary = (*p)["summary"].GetString();

				//Output formatted forecast
				std::cout << std::left << std::put_time(std::gmtime(&unixTime), "%c:") << " " 
						  << weathSummary << " " << (*p)["temperature"].GetFloat()
						  << "F " << std::endl;
			}
			fclose(fp);
			//****************rapidjson*******************
			//********************************************

	//Waits for input and deletes file with fullUrl
	delLink();

}//Main function

//Read given file
std::string getFile(std::string path) {
	std::string nLine;
	std::ifstream file_(path);

	if (file_.is_open()) {
		while (getline(file_, nLine)) {
			//std::cout << line_ << "\n";
			return nLine;
		}
		file_.close();
	}
	else {
		std::cout << "file is not open" << "\n";
		//std::cin.get();
		return "Error 0x000001: inaccesable file location";
	}
	return "Unknown error in function 'getFile()'"; //This should never happen
}

//Function to get user coordinates and verify the validity of them
std::string getCoords() {
	bool t = false;
	//Get user input loop
	do {
		float usrLat = 0;
		float usrLong = 0;/*
		std::cout << "Input latitude: ";
		std::cin >> usrLat;
		std::cout << "Input longitude: ";
		std::cin >> usrLong;*/
		std::cout << "Input postal code: ";
		std::cin >> POSTAL_CODE;

		std::string revGeoURL = "https://maps.googleapis.com/maps/api/geocode/json?address=" + POSTAL_CODE + "&key=" + getFile(GEO_PATH);
		char *charRevGeoURL = &revGeoURL[0u];

		//Save reverse geocoding url contents to file
		CURL *curl;
		FILE *fd;
		CURLcode res;
		char outfilename[FILENAME_MAX] = "debugOld/revoOld.json";
		curl = curl_easy_init();
		if (curl) {
			fd = fopen(outfilename, "wb");
			curl_easy_setopt(curl, CURLOPT_URL, charRevGeoURL);
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fd);
			res = curl_easy_perform(curl);
			curl_easy_cleanup(curl);
			fclose(fd);
		}

		//parse json file for postal code
		FILE* fp = fopen("../alertMe/debugOld/revoOld.json", "rb");
		char readBuffer[65536];//in bytes

		rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));
		rapidjson::Document d;
		d.ParseStream(is);

		//parse through "results" object of arrays
		const rapidjson::Value& prog = d["results"];
		
		for (rapidjson::Value::ConstValueIterator p = prog.Begin(); p != prog.End(); ++p) {
			std::cout << (*p)["formatted_address"].GetString();
			std::cout << "\n";
			const rapidjson::Value& preCoords = (*p)["geometry"]["location"];
			usrLat = preCoords["lat"].GetFloat();
			usrLong = preCoords["lng"].GetFloat();
			std::cout << "\n";
		}
			fclose(fp);

			//Checks if coordinates are valid
			if (usrLat < 90 && usrLat > -90 && usrLong < 180 && usrLong > -180) {
				std::string nestUsrCoords = std::to_string(usrLat) + "," + std::to_string(usrLong);
				t = false;
				INP_COORDS = nestUsrCoords;

				return nestUsrCoords;
			}
			else {
				std::cout << "Invalid\n";
				t = true;
			}
		} while (t == true);
		return "Unknown error in function 'getCoords()'"; //This should never happen
	}


//Function to write fullLink to a file
std::string writeLink(std::string fullLink) {

	//Future: get current working directory or host files on server
	std::string pchPath = "../alertMe/debugOld/pchDep.txt";
	std::ofstream pchDep(pchPath);
	pchDep << fullLink << std::endl;
	pchDep.close();
	return fullLink;
}

//Function to delete created file
void delLink() {
	system("pause");
	std::remove("debugOld/pchDep.txt");
}