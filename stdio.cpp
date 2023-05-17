#include <fcntl.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>
#include <strings.h>
#include <string.h>
#include <stdarg.h>   
#include <stdlib.h>    
using namespace std;

char decimal[100];

// recursive_itoa
// Recursive Integer to Char (ASCII) conversion.  Helper for *itoa
// Populates the decimal char array that represents a given int
int recursive_itoa(int arg) 
{
	int div = arg / 10;
	int mod = arg % 10;
	int index = 0;
	if (div > 0)
	{
		index = recursive_itoa(div);
	}
	decimal[index] = mod + '0';
	return ++index;
}

// *itoa
// Integer to character array (c string)
char *itoa(const int arg) 
{
	bzero(decimal, 100);
	int order = recursive_itoa(arg);
	char *new_decimal = new char[order + 1];
	bcopy(decimal, new_decimal, order + 1);
	return new_decimal;
}

// printf 
// Implementation of the STL printf function
// Takes a format string and a variable number of arguments
//  and prints the formatted output to the standard output stream
int printf(const void *format, ...) 
{
	va_list list; 					// variable argument list type
	va_start(list, format);

	char *msg = (char *)format;
	char buf[1024];
	int nWritten = 0;

	int i = 0, j = 0, k = 0;
	while (msg[i] != '\0') 
	{
		if (msg[i] == '%' && msg[i + 1] == 'd')
		{
			buf[j] = '\0';
			nWritten += write(1, buf, j);
			j = 0;
			i += 2;

			int int_val = va_arg(list, int);
			char *dec = itoa(abs(int_val));
			if (int_val < 0)
			{
				nWritten += write(1, "-", 1);
			}	
			nWritten += write(1, dec, strlen(dec));
			delete dec;
		}
		else
		{
			buf[j++] = msg[i++];
		}	 
	}
	if (j > 0)
	{
		nWritten += write(1, buf, j);
	}	
	va_end( list );
	return nWritten;
}

// setvbuf
// Sets the buffering mode and the size of the buffer for a stream
// Returns 0 if successful, EOF/-1 if using an unsupported mode
int setvbuf(FILE *stream, char *buf, int mode, size_t size) 
{
	if (mode != _IONBF && mode != _IOLBF && mode != _IOFBF)
	{
		return -1;
	}	
	stream->mode = mode;
	stream->pos = 0;
	if (stream->buffer != (char *)0 && stream->bufown == true)
	{
		delete stream->buffer;
	}
	
	switch ( mode ) 
	{
		case _IONBF:
			stream->buffer = (char *)0;
			stream->size = 0;
			stream->bufown = false;
			break;
		case _IOLBF:
		case _IOFBF:
			if (buf != (char *)0) 
			{
				stream->buffer = buf;
				stream->size   = size;
				stream->bufown = false;
			}
			else 
			{
				stream->buffer = new char[BUFSIZ];
				stream->size = BUFSIZ;
				stream->bufown = true;
			}
			break;
	}
	return 0;
}

// setbuf
// Sets the buffering mode and the size of the buffer for a stream
// Will use the defualt buffer size of 8192 if a buffer is null
void setbuf(FILE *stream, char *buf) 
{
	setvbuf(stream, buf, ( buf != (char *)0 ) ? _IOFBF : _IONBF , BUFSIZ);
}

// *fopen
// Opens a file with the specified mode and returns a file pointer. 
// It uses the open system call to open the file and sets the buffering mode to fully 
//  buffered with a buffer size of 8192 using setvbuf.
FILE *fopen(const char *path, const char *mode) 
{
	FILE *stream = new FILE();
	setvbuf(stream, (char *)0, _IOFBF, BUFSIZ);
	
	// fopen( ) mode
	// r or rb = O_RDONLY
	// w or wb = O_WRONLY | O_CREAT | O_TRUNC
	// a or ab = O_WRONLY | O_CREAT | O_APPEND
	// r+ or rb+ or r+b = O_RDWR
	// w+ or wb+ or w+b = O_RDWR | O_CREAT | O_TRUNC
	// a+ or ab+ or a+b = O_RDWR | O_CREAT | O_APPEND

  switch(mode[0]) 
  {
  case 'r':
	  if (mode[1] == '\0')            // r
	  {
		  stream->flag = O_RDONLY;
	  }  
	  else if ( mode[1] == 'b' ) 
	  {    
		  if (mode[2] == '\0')          // rb
		  {
			  stream->flag = O_RDONLY;
		  } 
		  else if (mode[2] == '+')      // rb+
		  {
			  stream->flag = O_RDWR;
		  }			  
	  }
	  else if (mode[1] == '+')        // r+  r+b
	  {
		  stream->flag = O_RDWR;
	  }  
	  break;
  case 'w':
	  if (mode[1] == '\0')            // w
	  {
		  stream->flag = O_WRONLY | O_CREAT | O_TRUNC;
	  }	  
	  else if (mode[1] == 'b') 
	  {
		  if (mode[2] == '\0')          // wb
		  {
			  stream->flag = O_WRONLY | O_CREAT | O_TRUNC;
		  }	  
		  else if (mode[2] == '+')      // wb+
		  {
			  stream->flag = O_RDWR | O_CREAT | O_TRUNC;
		  }	  
	  }
	  else if (mode[1] == '+')        // w+  w+b
	  {
		  stream->flag = O_RDWR | O_CREAT | O_TRUNC;
	  }
	  break;
  case 'a':
	  if (mode[1] == '\0')            // a
	  {
		  stream->flag = O_WRONLY | O_CREAT | O_APPEND;
	  } 
	  else if (mode[1] == 'b')
	  {
		  if (mode[2] == '\0')          // ab
		  {
			  stream->flag = O_WRONLY | O_CREAT | O_APPEND;
		  }  
		  else if (mode[2] == '+')      // ab+
		  {
			  stream->flag = O_RDWR | O_CREAT | O_APPEND;
		  }	  
	  }
	  else if (mode[1] == '+')        // a+  a+b
	  {
		  stream->flag = O_RDWR | O_CREAT | O_APPEND;
	  } 
	  break;
  }
  
  mode_t open_mode = S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;

  if ((stream->fd = open(path, stream->flag, open_mode)) == -1) 
  {
	  delete stream;
	  printf("fopen failed\n");
	  stream = NULL;
  }
  
  return stream;
}

/// @brief purges the buffer stream and resets the position
/// @param stream File pointer to purger
/// @return a 0 if successful purged
int fpurge(FILE *stream)
{
	if(stream == nullptr) {
      return 0;
   }else{
      stream->buffer = (char*) 0;
      stream->pos = 0;
      stream->actual_size = 0;
   }

   return 0;
}

/// @brief flushes the buffer to the file descriptor associated with stream
/// @param stream File pointer
/// @return 0 if successful, -1 if not
int fflush(FILE *stream) 
{
   //check to see if it needs to be flushed
	if(stream == nullptr || stream->flag == O_RDONLY)
	   return -1;

   //write bytes in buffer
   int writeBytes = write(stream->fd, stream->buffer, stream->pos);
   if(writeBytes == -1)
      return -1;
   
   stream->pos = 0;
   stream->actual_size = 0;
   return 0;
}  

/// @brief fread (Sample input/return parameter comments - For full behavior, consult C documentation for stdio functions)
///        Reads data from a given stream into a ptr buffer
/// @param ptr pointer to the buffer where the data read from the file will be stored
/// @param size the size(in bytes) of each element to be read
/// @param nmemb the number of elements that will be read from the file, each one with "size" bytes
/// @param stream pointer to the file to read from
/// @return total number of elements read.  (Note: can be less than requested items)
//			   (Note: size_t is an unsigned integer type that is often used as the return type to represent 
//			   returned sizes of objects.  By using size_t vs int, you can guarantee a non-neg value that can represent
//				the sizes of the largest objects possible in memory)
size_t fread(void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
	size_t readBytes = 0;
   size_t totalBytes = nmemb * size;

   //check flag to see if it is readable
   if(stream->flag == O_WRONLY)
      return 0;

   if(stream->mode != _IONBF){
      //check to see if the buffer is empty
      if(stream->pos >= stream->actual_size){
         //buffer is empty, read and fill buffer
         stream->actual_size = read(stream->fd, stream->buffer, stream->size);
         stream->pos = 0;
         if(stream->actual_size <= 0){
            stream->eof = true;
            return 0;
         }
      }
      
      //read data from buffer
      //first check to see that the requested bytesleft is small than the number of bytes left to read
      //if not then set to bytes left to read to bytesLeft
      size_t bytesLeft = totalBytes;
      if(bytesLeft > (stream->actual_size - stream->pos)) {
         bytesLeft = stream->actual_size - stream->pos;
      }

      //copy data from buffer to ptr (aka FILE)
      memcpy(ptr, stream->buffer + stream->pos, bytesLeft);
      stream->pos += bytesLeft;
      readBytes += bytesLeft;

   } else {

      //no buffer just read from file
      readBytes = read(stream->fd, ptr, totalBytes);
      if(readBytes <= 0) {
         stream->eof = true;
      }
   }

   //number of elements read
	return readBytes/size;
}

/// @brief fwrite, writes the data using the stream FILE
/// @param ptr pointer to the buffer where we will write to
/// @param size the size(in bytes) of each element to be read
/// @param nmemb the number of elements that will be read from the file, each one with "size" bytes
/// @param stream pointer to the file to read from
/// @return total number of elements written
size_t fwrite(const void *ptr, size_t size, size_t nmemb, FILE *stream) 
{
   if (stream == NULL || stream->fd < 0) {
      // Invalid file pointer or file descriptor
      return 0;
   }
   
   if(stream->flag == O_RDONLY)
      return 0;

	//Calculate bytes to write
   size_t totalBytes = size * nmemb;
   size_t writeBytes = 0;

   if(stream->mode != _IONBF){
      if(stream->pos + totalBytes >= stream->size)
         fflush(stream);
      
      //loop to ensure all bytes are written in case total grabbed is larger than buffer
      size_t remainBytes = totalBytes;
      size_t bufferSpace;
      while(remainBytes > 0){
         bufferSpace = stream->size - stream->pos;

         if(bufferSpace == 0){
            fflush(stream);
            bufferSpace = stream->size;
            stream->pos = 0;
         }
      
         //copy bytes either the whole buffer or whats left
         size_t bytesToCopy = (remainBytes < bufferSpace) ? remainBytes : bufferSpace;
         memcpy(stream->buffer + stream->pos, ptr, bytesToCopy);
         stream->pos += bytesToCopy;
         remainBytes -= bytesToCopy;
      }

      writeBytes = totalBytes;
   }else {
      writeBytes = write(stream->fd, ptr, totalBytes);
   }

   if(writeBytes <= 0)
      return -1;

   return writeBytes/size;
}

/// @brief gets a single character from the buffer
/// @param stream FILE pointer
/// @return the char value as an int or -1 if eof
int fgetc(FILE *stream) 
{
	//Check to see if buffer is empty
   if(stream->pos >= stream->actual_size){
      //Buffer is empty, read and fill buffer
      stream->actual_size = read(stream->fd, stream->buffer, stream->size);
      stream->pos = 0;

      if(stream->actual_size <= 0){
         stream->eof = true;
         return -1;
      }
   }
   //read a character from the buffer and incremenet pos
   int getchar = stream->buffer[stream->pos];
   stream->pos++;

	return getchar;
}

/// @brief puts a single character into the buffer
/// @param c char to insert
/// @param stream FILE pointer
/// @return the char value as an int or -1 if eof
int fputc(int c, FILE *stream) 
{
	//Check if buffer is full
   if(stream->pos >= stream->size){
      //buffer full flush it
      fflush(stream);
   }

   stream->buffer[stream->pos++] = c;

	return c;
}

/// @brief gets a string of characters from the buffer
/// @param str char array to fill
/// @param stream FILE pointer
/// @return the string as a char array pointer
char *fgets(char *str, int size, FILE *stream) 
{
	int readBytes = 0;
   
   while(readBytes < size - 1){
      //grab character from input buffer
      int c = fgetc(stream);

      //EOF check
      if(c == -1){
         if(readBytes == 0)
            return nullptr;
         else
            break;
      }

      //store in string
      str[readBytes] = (char)c;
      readBytes++;

      if(c == '\n')
         break;

   }

   str[readBytes] = '\0';
	return str;
}

/// @brief puts a string of characters into the buffer
/// @param str char array to insert
/// @param stream FILE pointer
/// @return 0 if succcessful
int fputs(const char *str, FILE *stream) 
{
	int writeBytes = 0;

   //check to see if buffer has enough space
   if(stream->pos + strlen(str) >= stream->size)
      fflush(stream);

   for(int i = 0; i < strlen(str); i++){
      int c = fputc((int)str[i], stream);
   }

	return 0;
}

/// @brief gets eof bool
/// @param stream FILE pointer
/// @return 0 if false 1 if true
int feof(FILE *stream) 
{
	return stream->eof == true;
}

/// @brief move the file pointer based off the offset and whence condition
/// @param stream FILE pointer
/// @param offset how far to move the file pointer
/// @param whence SEEK condtion SEEK_SET, SEEK_END, SEEK_CUR
/// @return 0 if succcessful
int fseek(FILE *stream, long offset, int whence) 
{
   //flush the buffer
   fflush(stream);

	if(lseek(stream->fd, offset, whence) == -1)
      return -1;
   
   stream->pos = 0;
   stream->actual_size = 0;
   stream->eof = false;
	return 0;
}

/// @brief close the file
/// @param stream FILE pointer to close
/// @return 0 if successful -1 if not
int fclose(FILE *stream) 
{
	if(stream == nullptr)
      return -1;
   
   //flush buffer
   fflush(stream);

   int fdClose = close(stream->fd);
   if(fdClose < 0)
      return -1;

   free(stream);
   
	return 0;
}
