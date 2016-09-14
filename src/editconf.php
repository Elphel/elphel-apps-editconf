<?php
/*!*******************************************************************************
*! FILE NAME  : editconf.php
*! DESCRIPTION: simple (not optimal) replacement of editconf.cgi
*!              to write/read internal parameters instead of the files
*!              to minimize camvc changes
*! Copyright (C) 2008 Elphel, Inc
*! -----------------------------------------------------------------------------**
*!
*!  This program is free software: you can redistribute it and/or modify
*!  it under the terms of the GNU General Public License as published by
*!  the Free Software Foundation, either version 3 of the License, or
*!  (at your option) any later version.
*!
*!  This program is distributed in the hope that it will be useful,
*!  but WITHOUT ANY WARRANTY; without even the implied warranty of
*!  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*!  GNU General Public License for more details.
*!
*!  You should have received a copy of the GNU General Public License
*!  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*! -----------------------------------------------------------------------------**
*!  $Log: editconf.php,v $
*!  Revision 1.1  2008/12/09 07:49:32  elphel
*!  added editconf.php - supplements editconf.cgi to deal with internal parameters, has the same interface to simplify interfacing from camvc
*!
*!
*/
 $sensor_port=0;
 $key=$_GET['key'];
 $value=$_GET['key'];
 $param=array();
 $param[$_GET['key']]=myval($_GET['value']);
 $xml = new SimpleXMLElement("<?xml version='1.0'?><editconf/>");
 if (array_key_exists('sensor_port',$_GET)) {
 	$sensor_port =  myval($_GET['sensor_port']);
 }
 if (array_key_exists('key',$_GET)) {
  if ($key=='ftp_left_time') {
     $param=elphel_get_P_arr($sensor_port,array('FTP_NEXT_TIME'=>0));
      $xml->addChild ('value',$param['FTP_NEXT_TIME']-time());
   } else if (array_key_exists('value',$_GET)) {
      $frame=elphel_set_P_arr($sensor_port,$param);
      $xml->addChild ('error',$frame?0:-3);
   } else {
      $param=elphel_get_P_arr($sensor_port,$param);
      $xml->addChild ('value',$param[$_GET['key']]);
   }
 } else {
      $xml->addChild ('error',0);
 }
 if (array_key_exists('sync',$_GET)) {
    exec ('sync');
     $xml->addChild ('sync','OK');
 }
 $rslt=$xml->asXML();
 header("Content-Type: text/xml");
 header("Content-Length: ".strlen($rslt)."\n");
 header("Pragma: no-cache\n");
 printf($rslt);

 exit (0);
function myval ($s) {
  $s=trim($s,"\" ");
  if (strtoupper(substr($s,0,2))=="0X")   return intval(hexdec($s));
  else return intval($s);
}
//http://192.168.0.7/editconf.php?&key=DAEMON_EN_CCAMFTP&value=1&eq=1&_time=1228790213230
?>
