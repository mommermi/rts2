#ifndef __RTS2_CONFIG__
#define __RTS2_CONFIG__

#ifdef _cplusplus
extern "C"
{
#endif

#define CFG_STRING	0
#define CFG_DOUBLE	1

  extern int read_config (char *filename);
  extern int get_string (char *name, char **val);
  extern char *get_string_default (char *name, char *def);
  extern int get_double (char *name, double *val);
  extern double get_double_default (char *name, double def);
  extern int get_device_string (char *device, char *name, char **val);
  extern char *get_device_string_default (char *device, char *name,
					  char *def);
  extern int get_device_double (char *device, char *name, double *val);
  extern double get_device_double_default (char *device, char *name,
					   double def);

#ifdef _cplusplus
}
#endif

#endif				/* __RTS2_CONFIG__ */
