#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>

//2021-11-19_19
int isLeapYear(int year)
{
    return ((year % 4 == 0) &&(year % 100 != 0)) || (year %  400 ==0);
}
int lastDay(int year, int mon)
{
    int mday[] ={ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    mday[1] = isLeapYear(year) ? 29 : 28;
    return mday[mon -1];
}
int checkdate(struct tm startD, struct tm endD)
{
    if(startD.tm_year > endD.tm_year)
        return 1;
    else if(startD.tm_year == endD.tm_year)
    {
        if(startD.tm_mon > endD.tm_mon)
            return 2;
        else if(startD.tm_mon == endD.tm_mon)
        {
            if(startD.tm_mday > endD.tm_mday)
                return 3;
            else if(startD.tm_mday == endD.tm_mday)
            {
                if(startD.tm_hour > endD.tm_hour)
                    return 4;
            }
        }
    }
    return 0;
}
struct tm increaseHour(struct tm *date)
{
    if(date->tm_hour < 23)
    {
        date->tm_hour++;
        return *date;
    }
    else    //data.tm_hour == 23
    {
        if(date->tm_mday == lastDay(date->tm_year, date->tm_mon))  //check now is last day
        {
            //add month
            if(date->tm_mon == 12)
            {
                //add year
                date->tm_year++;
                date->tm_mon = 1;
            }
            else
            {
                date->tm_mon++;
            }
            date->tm_mday = 1;
        }
        else
        {
            date->tm_mday++;
        }
        date->tm_hour = 0;
    }
    return *date;
}
int main (int argc, char *argv[])
{
    int i,j,k;
    FILE *fp = NULL;
    FILE *fpOut = NULL;
    char baseFileName[] ="logging_2021-11-";
    char loadFileName[30] = {0,};
    char outputFileName[50] ={0,};
    char findString[] = "ARHISMAIN Update Device Status:";
    int size = 0;
    char *buffer = NULL;
    char *bufptr = NULL;
    char *bufptr1 = NULL;
    char *tokptr = NULL;
    //char tempBuff[2000] = {0,};
    char *tempBuff = NULL;
    char temp[50] = {0,};
    int count = 0;
    int startNum[2] ={0,};
    int endNum[2] = {0,};
    struct tm start, finish;
    for(i=0; i<argc; i++)
    {
        printf("argc: %d, argv : %s \r\n", i, argv[i]);
    }
    //strcpy(temp, argv[1]);
    tokptr = strtok(argv[1], "-");
    if(tokptr ==NULL)
    {
        printf("not find error\r\n");
        return 1;
    }
    start.tm_year = atoi(tokptr);
    tokptr = strtok(NULL, "-");
    start.tm_mon = atoi(tokptr);
    tokptr = strtok(NULL, "_");
    start.tm_mday = atoi(tokptr);
    tokptr = strtok(NULL, "_");
    start.tm_hour = atoi(tokptr);
    printf("start %d-%d-%d_%d \r\n", start.tm_year, start.tm_mon, start.tm_mday, start.tm_hour);

    tokptr = strtok(argv[2], "-");
    if(tokptr ==NULL)
    {
        printf("not find error\r\n");
        return 1;
    }
    finish.tm_year = atoi(tokptr);
    tokptr = strtok(NULL, "-");
    finish.tm_mon = atoi(tokptr);
    tokptr = strtok(NULL, "_");
    finish.tm_mday = atoi(tokptr);
    tokptr = strtok(NULL, "_");
    finish.tm_hour = atoi(tokptr);
    printf("end %d-%d-%d_%d \r\n", finish.tm_year, finish.tm_mon, finish.tm_mday, finish.tm_hour);
    if(checkdate(start, finish) == 1)
    {
        printf("data input error \r\n");
        return -1;
    }
    
    sprintf(outputFileName,"log_%04d-%02d-%02d_%02d--%04d-%02d-%02d_%02d.txt",start.tm_year, start.tm_mon, start.tm_mday, start.tm_hour,finish.tm_year, finish.tm_mon, finish.tm_mday, finish.tm_hour);
    printf("output fileName : [%s]\r\n", outputFileName);
    fpOut = fopen(outputFileName,"w");
    // fputs(EncBuff,fpOut);
    // fclose(fpOut);
    i = 0;
    while(checkdate(start, finish) == 0)
    {
        printf("num %02d: %04d-%02d-%02d_%02d \r\n", i++, start.tm_year, start.tm_mon, start.tm_mday, start.tm_hour);
        sprintf(loadFileName,"logging_%04d-%02d-%02d_%02d.log",start.tm_year, start.tm_mon, start.tm_mday, start.tm_hour);
        printf("filename:[%s]\r\n",loadFileName);
        //load file
        fp = fopen(loadFileName,"r");
        if(fp ==NULL)
        {
            printf("File open Failed..\r\n");
            return 1;
        }
        //file size check
        fseek(fp, 0, SEEK_END);
        size = ftell(fp);
        printf("%s file size = %d \r\n",loadFileName, size);
        //file read data
        buffer = calloc(size + 1, sizeof(char));
        tempBuff = calloc(size + 1, sizeof(char));
        fseek(fp, 0, SEEK_SET);
        count = fread(buffer, size, 1, fp);        
        //pars data
        bufptr =strstr(buffer, findString);
        while(bufptr != NULL)
        {
            //printf("i=%d\r\n", i);
            bufptr1 =strstr(bufptr + 1, findString);
            //printf("bufptr = %c\r\n", bufptr[0]);
            strncat(tempBuff, bufptr-33 , 23);
            //printf(" pars data : [%s]\r\n", tempBuff);
            tokptr = strtok(bufptr, ":");
            strcat(tempBuff, "|");
            for(j=0;j<3;j++)
            {
                tokptr =strtok(NULL, "=");
                strcat(tempBuff, tokptr);
                strcat(tempBuff, "|");
                if(j==2)
                    tokptr =strtok(NULL, "\n");
                else
                    tokptr =strtok(NULL, ",");
                strcat(tempBuff, tokptr);
                strcat(tempBuff, "|");    
            }
            bufptr =strstr(bufptr1 + 1, findString);
            tokptr = strtok(bufptr1, ":");
            for(j=0;j<10;j++)
            {
                tokptr =strtok(NULL, "=");
                //strcat(tempBuff, "|");
                strcat(tempBuff, tokptr);
                strcat(tempBuff, "|");
                if(j==9)
                {
                    tokptr =strtok(NULL, "\n");
                    strcat(tempBuff, tokptr);
                    strcat(tempBuff, "|\r\n");
                }
                else
                {
                    tokptr =strtok(NULL, ",");
                    strcat(tempBuff, tokptr);
                    strcat(tempBuff, "|");
                }   
            }
        }
        printf("temp buffer = [\r\n%s]\r\n", tempBuff);
        //write file
        fputs(tempBuff,fpOut);
        free(tempBuff);
        free(buffer);
        fclose(fp);
        increaseHour(&start);
    }
    fclose(fpOut);

    #if 0
    if(startNum[0] > endNum[0])
    {
        printf("day fail \r\n");
        return -1;
    }
    else if(startNum[0] == endNum[0])
    {
        if(startNum[0] > endNum[0])
        {
            printf("Hour fail \r\n");
            return -2;
        }
    }
    for(i=startNum[0];i<=endNum[0];i++)
    {
        for(k=startNum[1];k<=endNum[1];k++)
        {

        }
    }
    #endif
    #if 0
    strcpy(loadFileName, baseFileName);
    strcat(loadFileName, argv[1]);
    strcat(loadFileName, ".log");
    printf("fileName : [%s] \r\n", loadFileName);
    #endif

    #if 0
    fp = fopen(loadFileName,"r");
    if(fp ==NULL)
    {
        printf("File open Failed..\r\n");
        return 1;
    }
    //file size check
    fseek(fp, 0, SEEK_END);
    size = ftell(fp);
    printf("file size = %d \r\n", size);
    //file read data
    buffer = calloc(size + 1, sizeof(char));
    tempBuff = calloc(size + 1, sizeof(char));
    fseek(fp, 0, SEEK_SET);
    count = fread(buffer, size, 1, fp);
    bufptr =strstr(buffer, findString);
    #endif
    //for (i=0; i<2; i++)
    #if 0
    while(bufptr != NULL)
    {
        //printf("i=%d\r\n", i);
        bufptr1 =strstr(bufptr + 1, findString);
        //printf("bufptr = %c\r\n", bufptr[0]);
        strncat(tempBuff, bufptr-33 , 23);
        //printf(" pars data : [%s]\r\n", tempBuff);
        tokptr = strtok(bufptr, ":");
        strcat(tempBuff, "|");
        for(j=0;j<3;j++)
        {
            tokptr =strtok(NULL, "=");
            strcat(tempBuff, tokptr);
            strcat(tempBuff, "|");
            if(j==2)
                tokptr =strtok(NULL, "\n");
            else
                tokptr =strtok(NULL, ",");
            strcat(tempBuff, tokptr);
            strcat(tempBuff, "|");    
        }
        #if 0
        tokptr =strtok(NULL, "=");
        printf("active_mod N: [%s]\r\n", tokptr);
        strcat(tempBuff, "|");
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, ",");
        printf("active_mod V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");

        tokptr =strtok(NULL, "=");
        printf("run_mod N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, ",");
        printf("run_mod V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");

        tokptr =strtok(NULL, "=");
        printf("device_mod N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, "\n");
        printf("device_mod V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        #endif
        //===next Line
        //strncat(tempBuff, bufptr1-33 , 23);
        //printf("bufptr = %c\r\n",bufptr1[0]);
        bufptr =strstr(bufptr1 + 1, findString);
        tokptr = strtok(bufptr1, ":");
        for(j=0;j<10;j++)
        {
            tokptr =strtok(NULL, "=");
            //strcat(tempBuff, "|");
            strcat(tempBuff, tokptr);
            strcat(tempBuff, "|");
            if(j==9)
            {
                tokptr =strtok(NULL, "\n");
                strcat(tempBuff, tokptr);
                strcat(tempBuff, "|\r\n");
            }
            else
            {
                tokptr =strtok(NULL, ",");
                strcat(tempBuff, tokptr);
                strcat(tempBuff, "|");
            }   
        }
        #if 0
        tokptr =strtok(NULL, "=");
        printf("batt_type N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, ",");
        printf("batt_type V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, "=");
        printf("temp N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, ",");
        printf("temp V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, "=");
        printf("humi N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, ",");
        printf("humi V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, "=");
        printf("sd N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, ",");
        printf("sd V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, "=");
        printf("batt p N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, ",");
        printf("batt p V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, "=");
        printf("batt v N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, ",");
        printf("batt v V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, "=");
        printf("mem N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, ",");
        printf("mem V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, "=");
        printf("cpu N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, ",");
        printf("cpu V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, "=");
        printf("net N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, ",");
        printf("net V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, "=");
        printf("cpu t  N: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|");
        tokptr =strtok(NULL, "\n");
        printf("cpu t  V: [%s]\r\n", tokptr);
        strcat(tempBuff, tokptr);
        strcat(tempBuff, "|\r\n");
        #endif
    }
    printf("temp buffer = [\r\n%s]\r\n", tempBuff);
    #endif
    #if 0
    free(tempBuff);
    free(buffer);
    fclose(fp);
    #endif
    return 0;
}
//2021-11-19 15:00:33,341 - INFO - ARHISMAIN Update Device Status: active_mode=Normal, run_mode=Normal, device_mode=deviceNormal
//2021-11-19 15:00:33,351 - INFO - ARHISMAIN Update Device Status: batt_type=1, temp=45.1, humi=6.8, sd=31, batt_per=90, batt_vol=11.99262272, mem=15.9, cpu=86.3, net=97.4, cpu_temp=73.0