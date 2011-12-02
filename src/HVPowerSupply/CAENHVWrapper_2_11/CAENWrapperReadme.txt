/*****************************************************************************/
/*                                                                           */
/*                  --- CAEN SpA - Computing Division ---                    */
/*                                                                           */
/*   CAEN HV Wrapper Library Rel. 2.11 Installation and Use Instructions     */
/*									     */
/*   July    2003	                 				     */
/*                                                                           */
/*****************************************************************************/
  
 This archive contains the last release of the CAEN HV Wrapper Library and the
 corresponding Release Notes.

 The complete documentation can be found in the CAEN HV Wrapper Library Manual
 available once installed the Library or on CAEN's Web Site 
 at www.caen.it.


 Content of the archive
 ----------------------

 CAENWrapperReadme.txt        :  This file
 CAENWrapperReleaseNotes.txt  :  Release Notes of the last software release
 Makefile      		      :  makefile for library installation and demo 
                                 program
 
 Lib/
  libcaenhvwrapper.so.2.11    :  executable of the library
  hscaenetlib.so.1.6          :  executable of the HSCAENETLib library to 
                                 control A303A and A1303 simultaneously
  
 Doc/
  CAENHVWrapper-2-11.pdf       :  user's manual of the library
 
 include/
  CAENHVWrapper.h             :  include file for use of the library
  caenhvoslib.h		      :  accessory include file
 
 HVWrapperDemo/               :  directory with sources/executable of the demo 
                                 program 
 


 System Requirements
 -------------------
 
 - Network Interface Card + TCP/IP protocol (to control SY 1527/ SY 2527)
 - A303A/A1303 H.S. CAENET Controller Card (to control SY 527, SY 127, SY 403,
   N 470, N 570 and N568)
 - A128HS SY127 H.S. CAENET Controller installed on SY 127
 - SY 1527/ SY 2527 firmware version 1.10.0 or later (recommended 1.14.03)
 - SY403 firmware version 1.45 or later (recommended 1.46)
 - SY527 firmware version 4.03 or later
 - Linux kernel 2.2 or 2.4 with gnu C/C++ compiler


 Installation notes
 ------------------

 1. It's necessary to login as root

 2. Copy the file CAENHVWrapper-2-11.tgz in your work directory
 
 3. execute the command tar xzvf CAENHVWrapper-2-11.tgz
 
 4. execute: make install
 
 The installation copies and installs the library in /usr/lib, compiles the demo 
 program and installs it in the work directory.
 To use the demo program, change to the demo program directory and launch the
 application typing ./HVWrappdemo.
   

 Note:
 -----
 Control of CAEN Power Supplies via CaeNet link requires the correct
 installation of the A303 and/or A1303 device driver. 


