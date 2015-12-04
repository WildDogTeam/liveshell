/*
 * Copyright (C) 2014-2016 Wilddog Technologies. All Rights Reserved. 
 *
 * FileName: liveshell.c
 *
 * Description: liveshell file.
 *
 * Usage: liveshell    [option] <your wilddog url> 
 *                      <your callback command> 
 *      
 *
 * History:
 * Version      Author          Date                    Description
 *
 * 1.0.0        Baikal.Hu       2015-09-28              Create file.
 *
 */

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <string.h> 
#include <sys/types.h>
#include "wilddog.h"


#define  LIVESHELL_VERSION	"1.4" 

STATIC void auth_callback
    (
        void* arg,
        Wilddog_Return_T err
    )
{
    *(BOOL*)arg = TRUE;
    if(err < WILDDOG_HTTP_OK || err >= WILDDOG_HTTP_NOT_MODIFIED)
    {
        wilddog_debug("auth failed!;error =%d \n",err);
        return;
    }
}


Wilddog_Str_T * getHostFromUrl(Wilddog_Str_T *url)
{
	int length = 0;
	int m = 0, n = 0;
	int i = 0;
	Wilddog_Str_T *host = NULL;

	length = strlen(url);
	for(i = 0; i < length; i++)
	{
		if(*(url+i) == '/' && *(url+i+1) == '/')
			break;
	}

	m = i+2;

	for(i = m; i < length; i++)
	{
		if(*(url+i) == '/')
			break;
	}
	n = i-1;

	host = wmalloc(n-m+2);
	if(NULL == host)
	{
		
	}

	strncpy(host, url+m, n-m+1);
	return host;
}


typedef struct _watch_ctx
{
    BOOL *isFinished;
    char *cmd;
    char *data;
    char *verbose;
    char *quote;
}WATCH_CTX;

STATIC void observer_callback
    (
    const Wilddog_Node_T* p_snapshot, 
    void* arg,
    Wilddog_Return_T err
    )
{
    int ret = 0;
    char cmd[1024];
    static int times=0;
    pid_t pid;
    char * pstr = NULL;
    char * pleaf = NULL;
    Wilddog_Node_T *p_clone = NULL;
    Wilddog_Str_T *p_key = NULL;
    char *d = " ";
    char *ps = NULL;
    char *args[0];
    int i = 0,j = 1;

    memset(cmd,0,1024);
    *((*(WATCH_CTX*)arg).isFinished)= TRUE;
    if(err < WILDDOG_HTTP_OK || err >= WILDDOG_HTTP_NOT_MODIFIED)
    {
        wilddog_debug("addObserver failed!");
        return;
    }

    if(strncmp("yes", (*(WATCH_CTX*)arg).data, strlen("yes")) == 0 && times == 0)
    {
        times++;
        return;
    }

    pid = fork();
    if(pid == -1)
        perror("fork");

    if(!pid)
    {
        p_clone = wilddog_node_clone(p_snapshot);
        p_key = p_clone->p_wn_key;
        p_clone->p_wn_key = NULL;
        pstr = (char *)wilddog_debug_n2jsonString(p_clone);
        fflush(stdout);
        /*Handle cmd's arg, add to arg table. Such as "ls -l", "-l" is an arg*/
        
        if((strncmp("no", (*(WATCH_CTX*)arg).quote, strlen("no")) != 0) && p_clone->p_wn_child == NULL && p_clone->d_wn_type == WILDDOG_NODE_TYPE_UTF8STRING)
        {
            /*If node is leaf and value is string, seperate to args by space*/
            pleaf = wmalloc(strlen(pstr)-1);
            memcpy(pleaf, pstr+1, strlen(pstr)-2);
            *(pleaf+strlen(pleaf)) = '\0';
            args[0] = strtok( (char *)((*(WATCH_CTX*)arg).cmd), d);
            while(ps = strtok(NULL,d))
            {
                args[j++] = ps;
            }
            args[j++] = strtok(pleaf, d);
            while(ps = strtok(NULL,d))
            {
                args[j++] = ps;
            }

            args[j] = NULL;
        }
        else
        {
            args[0] = strtok( (char *)((*(WATCH_CTX*)arg).cmd), d);
            while(ps = strtok(NULL,d))
            {
                args[j++] = ps;
            }
            
            args[j] = wmalloc(strlen(pstr)+1);
            memcpy(args[j], pstr, strlen(pstr));
            args[++j] = NULL;
        }

        if(strncmp("yes", (*(WATCH_CTX*)arg).verbose, strlen("yes")) == 0)
        {

            printf("exec this cmd and param: ");   
            for(i = 0; i < j; i++)
            {
                printf("%s ", args[i]);
                fflush(stdout);
            }
            printf("\n\n");
            fflush(stdout);
        }
        
        ret = execvp((char *)((*(WATCH_CTX*)arg).cmd), args);  
        wfree(pstr);
        wfree(pleaf);
        for( i = 0; i < j+1; i++)
            wfree(args[i]);
        p_clone->p_wn_key = p_key;
        wilddog_node_delete(p_clone);
        if(ret == -1)
        {
            perror("execv");
            exit(EXIT_FAILURE);
        }
    }
    
    return;
}


int main(int argc, char **argv) 
{
    char url[1024];
    char cmd[256];
    char data[32];
    char verbose[32];
    char quote[32];
    char authvalue[256];
    
    BOOL isFinished = FALSE;
    Wilddog_T wilddog = 0;
    int opt;
    int option_index = 0;
    WATCH_CTX watch_ctx;
    char *host = NULL;
    
    memset(url,0,sizeof(url));  
    memset(cmd,0,sizeof(cmd));
    memset(data,0,sizeof(data));
    memcpy(data, "yes",strlen("yes"));
    memset(verbose,0,sizeof(verbose));
    memcpy(verbose, "no",strlen("no"));
    memset(quote,0,sizeof(quote));
    memcpy(quote, "yes",strlen("yes"));
    memset(authvalue,0,sizeof(authvalue));
	
    static struct option long_options[] = 
    {
        {"ignore-origin-data",     required_argument, 0,  0 },
        {"verbose", required_argument, 0,  0},
        {"ignore-leaf-quote", required_argument, 0,  0},
        {"version", no_argument, 0,  0},
        {"authvalue", required_argument, 0,  0},
        {0,         0,                 0,  0 }
    };

    while ((opt = getopt_long(argc, argv, "ovsh",long_options, &option_index)) != -1) 
    {
        switch (opt) 
        {
        case 0:
            //printf("option!!! %s\n", long_options[option_index].name);
            if (optarg)
            {
                if(strcmp(long_options[option_index].name,"ignore-origin-data")==0)
                    memcpy(data, optarg,strlen(optarg));
                if(strcmp(long_options[option_index].name,"verbose")==0)
                    memcpy(verbose, optarg,strlen(optarg));
                if(strcmp(long_options[option_index].name,"ignore-leaf-quote")==0)
                    memcpy(quote, optarg,strlen(optarg));
                if(strcmp(long_options[option_index].name,"authvalue")==0)
                    memcpy(authvalue, optarg,strlen(optarg));
            }
	    if(strcmp(long_options[option_index].name,"version")==0)
	    {
		    printf("liveshell Version: %s\n", LIVESHELL_VERSION);	
		    return 0;
	    }
            break;
	case 'o':
	    memcpy(data, "no",strlen("no"));
	    break;
	case 'v':
	    memcpy(verbose, "yes",strlen("yes"));
	    break;
	case 's':
	    memcpy(quote, "no",strlen("no"));
	    break;
		
        case 'h':
            fprintf(stderr, "Usage: %s [option] [--authvalue= your auth data] <your wilddog url>  <your callback command> \n",
                   argv[0]);
            return 0;
        default: /* '?' */
            fprintf(stderr, "Usage: %s [option] [--authvalue= your auth data] <your wilddog url>  <your callback command> \n",
                   argv[0]);
            return 0;
        }
    }
	

    if( argc < 3 )
    {
        printf("Usage: %s [option] [--authvalue= your auth data] <your wilddog url>  <your callback command> \n", argv[0]);
        return 0;
    }
	
    memcpy(cmd, argv[argc-1], strlen(argv[argc-1]));
    memcpy(url, argv[argc-2], strlen(argv[argc-2]));	
	
    wilddog = wilddog_initWithUrl((Wilddog_Str_T *)url);

    if(0 == wilddog)
    {
        wilddog_debug("new wilddog failed!");
        return 0;
    }
    if(strlen(authvalue) != 0)
    {
	    host = getHostFromUrl(url);
	    //printf("host:%s\n", host);
	    //printf("authvalue:%s\n", authvalue);
	    wilddog_auth((u8*)host,(u8*)authvalue,strlen((const char *)authvalue),auth_callback,(void*)&isFinished);
    }
    
    isFinished = FALSE;
    watch_ctx.cmd = cmd;
    watch_ctx.data = data;
    watch_ctx.verbose = verbose;
    watch_ctx.quote = quote;
    watch_ctx.isFinished = &isFinished;
    
    wilddog_addObserver(wilddog, WD_ET_VALUECHANGE, observer_callback, (void*)&watch_ctx);
    while(1)
    {
        if(TRUE == *(watch_ctx.isFinished))
        {
            //wilddog_debug("get new data %d times!", count++);
            *(watch_ctx.isFinished) = FALSE;
            /*do not break*/
            //break;
        }
        wilddog_trySync();
    }
    wfree(host);
    wilddog_destroy(&wilddog);
    
    return 0;
}

