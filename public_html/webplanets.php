<HTML>
<BODY bgcolor="black" text="white" background="back/homeback.jpg" bgproperties="fixed">
<HEAD>
<META HTTP-EQUIV="Refresh" CONTENT="45; URL=webplanets.php">
</HEAD>
<br><br>
<font face="Courier" size="3">
<b>
<PRE>
<?
    $file = "WEBPLANETS";
  $fcontents = file($file);
  for ($i=0;$i<=sizeof($fcontents);$i++)
  {
    print($fcontents[$i]);
  }
?>
</PRE>
</b>
</font>
</BODY>
</HTML>

