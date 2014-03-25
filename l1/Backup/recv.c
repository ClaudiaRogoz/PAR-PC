/*Rogoz Claudia Rafaela 321CA
*Tema1 Protocoale de Comunicatie
*PAR Protocol 
*
*Receiver
*
*/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include "lib.h"
#include <time.h>

#define CHAR_BIT 8
#define HOST "127.0.0.1"
#define PORT 10001
#define MASK 128
#define MAX_SIZE 60

/* sequence number or ack number */
typedef int seq_nr;

/* struct used to compute the final message */
typedef struct{
	int len;
	char payload[2];
}frame;

/* returns the i-th bit */
int get_bit(char c, int i){
	return (c & (1<<i) >> i);
}

/* converts char to binary */
char* char_to_binary(char i)
{
    size_t bits = sizeof(char) * CHAR_BIT;

    char * str = malloc(bits + 1);
    if(!str) return NULL;
    str[bits] = 0;

    unsigned u = *(unsigned *)&i;
    for(; bits--; u >>= 1)
    	str[bits] = u & 1 ? '1' : '0';

    return str;
}


int main(int argc,char** argv){
  msg r,t;
  
  /* time stamp used for log file */
  time_t current_time;
  char* c_time_string;
  char sum ='\0';
  char* buffer;
  static int ok = 0;
  
  init(HOST,PORT);
 
  
  int i;	
  
  /* log file */
  FILE *ff = fopen("log.txt","w");
  FILE* e = fopen("date.out", "w");
  fclose(ff);
  
  /* The initial package is reassembled in this file */
  
  /* expected frame to arrive */
  seq_nr frame_expected = 0;  

  
  while (1)
  {
      
      if (recv_message (&r) < 0)
      {
          perror ("Recieve message");
		  return -1;
      }
	  ok++;
	  /* sends data to log file */
      current_time = time(NULL);
	  c_time_string = ctime(&current_time);
	  ff = fopen("log.txt", "a+");
	  fprintf(ff, "[%s] [receiver] Am primit urmatorul pachet:\n",c_time_string);
	  fprintf(ff, "Seq No: %d\n", (int)r.payload[0]);
	  fprintf(ff, "Payload: ");
	  
	  for (i = 1; i< r.len -1; i++){
		fprintf(ff,"%c",r.payload[i]);
		}
	  fprintf(ff,"\n");
	  fclose(ff);
	  /* if the arrived message is the expected one */
      if (r.payload[0] == (char)frame_expected)
		{
		
		/* checksum is checked ( the integrity of the messge) */
		sum = '\0';
  		for (i = 0; i<=r.len-2; i++)
			sum ^= r.payload[i];
		
		/* if the checksum is OK */
		if (sum == r.payload[r.len -1]){
			/* the expected frame becomes the next one in row*/
			 frame_expected += 1;
			 /* writes received data to output file */
			 memset(&buffer,'\0', MAX_SIZE);
			 e = fopen("date.out", "a+");
			  for (i = 1; i< r.len -1; i++){
				fprintf(e,"%c",r.payload[i]);
				}
			 fclose(e);
			
			
			 
			}		
		else {
			/* a checksum error appeared */
			ff = fopen("log.txt", "a+");
			fprintf(ff,"[CHECKSUM ERROR] \n Se trimite ACK pentru Seq No %d\
			(ultimul cadru corect pe care l-am primit)\n", frame_expected-1);
			fclose(ff);
			}
		}
	  else {
			/* the frame received is not the one expected */
			ff = fopen("log.txt", "a+");
			fprintf(ff,"[receiver]Cadrul trimis nu este cel dorit. Voi trimite ACK pentru seq No %d \
			(ultimul cadru corect pe care l-am primit)\n", frame_expected -1);
			fprintf(ff,"Seq No: %d\n", frame_expected -1);
			fprintf(ff,"----------------------------------------------------------------\n");
			fclose(ff);
			}
			
	  /*sends the ack or nack based on the seq_no */
	  t.payload[0] = frame_expected -1;
	  t.payload[1] = frame_expected -1;
			
	  /* sends informations to log file */
	  current_time = time(NULL);
	  c_time_string = ctime(&current_time);
	  ff = fopen("log.txt", "a+");
	  fprintf(ff, "[%s] [receiver] Trimit ACK:\n",c_time_string);
	  fprintf(ff, "Seq No: %d\n", (int)(t.payload[0]));
	  fprintf(ff, "Checksum: %s\n", char_to_binary(t.payload[0]));
	  fprintf(ff, "----------------------------------------------------------------\n");
	  fclose(ff);
	  
	  
	  /* forces a delay*/
	   //if( ok == 10)
	   //{
		//sleep(2);ok+= 2;
	   //}
	  /* the ack/nack message is sent */
	  send_message(&t);
	}
  
  return 0;
}
