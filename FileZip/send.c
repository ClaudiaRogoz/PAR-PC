/*Rogoz Claudia Rafaela 321CA
*Tema1 Protocoale de Comunicatie
*PAR Protocol 
*
*Sender
*
*/


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <setjmp.h>
#include "lib.h"

#define CHAR_BIT 8
#define HOST "127.0.0.1"
#define PORT 10000

#define MAX_SIZE 60
#define CHAR_BIT 8
#define MASK 128


/* sequence number or ack number */
typedef int seq_nr;

/* struct used to compute the final message */
typedef struct{
	int len;
	seq_nr seq;
	char payload[MAX_SIZE+1];
	char checksum;
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


/* generates random size between 1 and 60 bytes */
int generate_random(){
	return 1+ rand() % MAX_SIZE;	
}


int main(int argc,char** argv){

  init(HOST,PORT);
  msg t;
  msg* rr;
  /* log file */
  FILE* ff;
  
 
  /* time stamp used for log file */
  time_t current_time;
  seq_nr next_frame_to_send;
  frame s;
  char* c_time_string;
  int i;
  jmp_buf env;
  //int val;
  
  char sum ='\0';
  
  /* file from where the information is read */
  int fd = open ("date.in", O_RDONLY);
  
  
 
  s.len = s.seq = 0;
  
  /* seq_no of the frame needed to be sent */
  next_frame_to_send = 0; 
  
  /* reads a random number of bytes to be computed */
  int rand = generate_random();
  size_t count = read(fd, s.payload, rand);
  setjmp (env); 
    
  while (1)
  {		
	/* the frame length is incremented */
	s.len = count + 2;
	
	/* the payload isinitialized*/
	memset(&t.payload,'\0',sizeof(t.payload));
	
	/* writes seq_no to the payload*/ 
	t.payload[0] = (char)next_frame_to_send ;
	
	/* writes data */
	for (i = 1; i<= count; i++)
		t.payload[i] = s.payload[i-1];

	/* computes the checksum */
	sum = '\0';
	for (i = 0; i<=count; i++)
		sum ^= t.payload[i];
	
	t.payload[s.len - 1] = sum;
	t.payload[s.len] = '\0';
	t.len = s.len;

	/*writes to log file*/
	ff = fopen ("log.txt", "a+");
	current_time = time(NULL);
	c_time_string = ctime(&current_time);
	
	fprintf(ff,"[%s] [sender] Am trimis urmatorul pachet:\n",c_time_string);
	fprintf(ff,"Seq No: %d\n", (int)t.payload[0]);
	fprintf(ff,"Payload: %s",s.payload);
	fprintf(ff,"\n");
	fprintf(ff,"Checksum: %s\n", char_to_binary(sum));
	fprintf(ff,"----------------------------------------------------------------\n");
	
	current_time = time(NULL);
	c_time_string = ctime(&current_time);
	fprintf(ff,"[%s] [sender] Am pornit cronometrul (timeoutul este de 25 ms):\n",c_time_string);
	fclose(ff);
	
	/* the message is beeing sent */
	send_message(&t);
	  
	/* waits the message from the receiver*/
	  rr = NULL;
	  rr = receive_message_timeout(25);
	 
	  /* checks if timeout event occours or not */
	  /*if the message arrived in time */
	  if(rr != NULL){
			
			/* verifies the seq_no */
			if ((*rr).payload[0] == next_frame_to_send){
				
				/* if the checksum for the message is ok */
				 if((*rr).payload[0] == (*rr).payload[1]){
					
					/*reads the next bytes to be computed */
					rand = generate_random();
					memset(&s.payload,'\0',MAX_SIZE);
					count = read(fd, s.payload, rand);
			        setjmp(env);		
					/* if there is nothing to be computed, it exits */
					if (count < 1  ) break;
					
					/* else the sequence is increased */
					next_frame_to_send += 1;
					
					/* sends info to log file */
					ff = fopen ("log.txt", "a+");
					fprintf(ff,"[sender] Am primit ok totul, incepe trimiterea cadrului %d\n", 
							next_frame_to_send);
					fclose(ff);
					}
				 else {
					/* if the received message is corrupt */
					ff = fopen ("log.txt", "a+");
					fprintf(ff,"[sender] Se retrimite pachetul No Seq: %d\n", next_frame_to_send);
					fprintf(ff,"[sender] Payload-ul primit este: %c\n",(*rr).payload[0]);
					fclose(ff);
				 }
			}	
			}
	  else {
                
				/* a timeout event occoured */
				 ff = fopen ("log.txt", "a+");
				 fprintf(ff,"[sender] TIMEOUT: Se retrimite pachetul cu Seq No %d\n", next_frame_to_send);
				 fclose(ff);
                 
		
		}
	
	}
	close (fd);
	
  return 0;
}
