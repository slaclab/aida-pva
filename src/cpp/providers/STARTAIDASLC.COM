$!       **CMS**=COM_GEN
$!
$!===============================================================================
$! @file
$!
$! Name:  STARTAIDASLC.COM
$!
$! @brief  Starts an AIDA-PVA Channel Provider or EPICS forwarder on VMS.
$!
$!
$!===============================================================================
$!
$! Set the correct java version 1.5 for running AIDA-PVA
$ @DISK$JAVA:[JAVA$150.COM]JAVA$150_SETUP.COM
$!
$! Make intermediate logicals to pass system properties
$! and avoid the VMS DCL 255 char limit.
$!
$ portconf == ""
$
$ if f$trnlnm ("AIDA_MODE") .eqs. "PROD"
$ then
$   portconf == " -DEPICS_PVA_BROADCAST_PORT=5077 "
$ endif
$
$ IF procname .EQS. "EPICS_FORWARDER" THEN GOTO STARTFORWARDER
$ libpath == "-Djava.library.path=/SLCSHR"
$ conf == portconf + " -Daida.pva.channels.filename=/SLCTXT/''config'"
$ libname = procname - "_"
$ lib == "-Daida.pva.lib.name=''libname'"
$!
$!
$ write sys$output "Starting ''procname' Server"
$       java_argstring = "-jar $conf $libpath $lib SLCLIBS:AIDA-PVA.JAR"
$       @slccom:RUNJAVA_BUT_RESTART_ON_ABORT
$       exit
$!
$STARTFORWARDER:
$ write sys$output "Starting FORWARDER Server"
$       java_argstring = "-jar $portconf SLCLIBS:EPICS_FORWARDER.JAR"
$       @slccom:RUNJAVA_BUT_RESTART_ON_ABORT
$       exit
