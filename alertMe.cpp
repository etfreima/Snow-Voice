#include "pch.h"
#include <stdio.h>
#include <cstdio>
#include <iostream>
#include <string>
#include <fstream>
#include <ctime>
#include <iomanip>
#include <istream>
#include "libs/rapidjson/document.h"
#include "libs/rapidjson/stringbuffer.h"
#include "libs/rapidjson/writer.h"
#include "libs/rapidjson/filereadstream.h"
#include "libs/vcpkg/buildtrees/cpprestsdk/src/v2.10.10-dc225cdbd9/Release/include/cpprest/http_client.h"
#include <unordered_map>


//#include "curl/curl.h"
//#include "curl/easy.h"
#include "C:/Users/anon/curl/builds/libcurl-vc-x64-release-dll-ipv6-sspi-winssl/include/curl/curl.h"
#include "C:/Users/anon/curl/builds/libcurl-vc-x64-release-dll-ipv6-sspi-winssl/include/curl/easy.h"
#pragma comment (lib, "C:/Users/anon/curl/builds/libcurl-vc-x64-release-dll-ipv6-sspi-winssl/lib/libcurl.lib")

#define CURL_STATICLIB

#pragma warning(disable:4996)

/*#ifdef _DEBUG
#	pragma comment (lib, "curl/libcurl_a_debug.lib")
#else
#	pragma comment (lib, "curl/libcurl_a.lib")
#endif
*/
//Pre declaration of functions, are defined under main function
std::string getCoords();
std::string writeLink(std::string fullLink);
void delLink();

int main()
{
	//Important variable declarations
	std::string preURL = "https://api.darksky.net/forecast/";
	std::string usrCoords;
	std::string line_;
	std::string fullURL;

	std::ifstream file_("../alertMe/debugOld/infLog.txt");
	if (file_.is_open()) {
		while (getline(file_, line_)) {
			//std::cout << line_ << "\n";
		}
		file_.close();
	}
	else {
		std::cout << "file is not open" << "\n";
		std::cin.get();
	}
	usrCoords = getCoords();

	//Generates link with user coordinates
	fullURL = preURL + line_ + "/" + usrCoords;
	char *charUrl = &fullURL[0u];
	char *charPreUrl = &preURL[0u];

	//Writes the link to a file
	std::cout << writeLink(fullURL);

	//New line
	std::cout << std::endl;


			//********************************************
			//********************cURL********************
			CURL *curl;
			FILE *fd;
			CURLcode res;
			char outfilename[FILENAME_MAX] = "debugOld/repoOld.json";
			curl = curl_easy_init();
			if (curl) {
				fd = fopen(outfilename, "wb");
				curl_easy_setopt(curl, CURLOPT_URL, charUrl);
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
						  << "C " << std::endl;
				
			}

			fclose(fp);
			//****************rapidjson*******************
			//********************************************

	//Waits for input and deletes file with link
	delLink();

}//Main function

//Function to get user coordinates and verify the validity of them
std::string getCoords() {
	bool t = false;

	//Get user input loop
	do{
		float usrLat = 0; 
		float usrLong = 0;
		std::cout << "Input latitude: ";
		std::cin >> usrLat;
		std::cout << "Input longitude: ";
		std::cin >> usrLong;

		//Checks if coordinates are valid
		if (usrLat < 90 && usrLat > -90 && usrLong < 180 && usrLong > -180) {
			std::string nestUsrCoords = std::to_string(usrLat) + "," + std::to_string(usrLong);
			t = false;
			return nestUsrCoords;
		}
		else {
			std::cout << "Invalid\n";
			t = true;
		}
	} while (t == true);
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