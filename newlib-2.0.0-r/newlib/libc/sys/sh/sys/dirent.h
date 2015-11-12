/* <dirent.h> includes <sys/dirent.h>, which is this file.  On a
   system which supports <dirent.h>, this file is overridden by
   dirent.h in the libc/sys/.../sys directory.  On a system which does
   not support <dirent.h>, we will get this file which uses #error to force
   an error.  */

#ifdef __cplusplus
extern "C" {
#endif


  struct dirent {
    ino_t d_ino;
    char   d_name[255];
  };

  typedef long DIR;

  int            closedir(DIR *);
  DIR           *opendir(const char *);
  struct dirent *readdir(DIR *);
  int            readdir_r(DIR *, struct dirent *, struct dirent **);
  void           rewinddir(DIR *);
  void           seekdir(DIR *, long int);
  long int       telldir(DIR *);

#ifdef __cplusplus
}
#endif
