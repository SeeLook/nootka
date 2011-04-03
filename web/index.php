<?
  print "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
  print "<?xml-stylesheet href=\"style.css\" type=\"text/css\"?>\n";
  print "<!DOCTYPE html PUBLIC \"-//W3C//DTD XHTML 1.0 Transitional//EN\" \"http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd\">\n";
  
     if ( isset($_GET["L"]) ) {
        switch($_GET["L"]) {
            case 'pl':
            case 'en':
                $Lang = $_GET["L"]; break;
            default:
                $Lang = 'en';
        }
    } else {
	$Blang = explode(';',$_SERVER['HTTP_ACCEPT_LANGUAGE']);
        $Blang = explode(',',$Blang[0]);
        $Blang = explode('-',$Blang[0]);
	switch ($Blang[0])	{
            case "pl" : $Lang = "pl"; break;
            default: $Lang = "en";
	}
     }
     
     if ( isset($_GET["C"]) ) {
        switch ($_GET["C"]) {
            case 'about':
            case 'scr':
            case 'down':
              $Contain = $_GET["C"];
              break;
            default:
                $Contain = 'about';
        }
    } else
      $Contain = 'about';
?>

<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="<? print $Lang ?>" lang="<? print $Lang ?>">
<head>
  <title>Nootka</title>
  <meta name="GENERATOR" content="Quanta Plus" />
  <meta http-equiv="Content-Type" content="text/html; charset=utf-8" />
  <meta name="KEYWORDS" content="" />
  <meta http-equiv="Content-Language" content="<? print "$Lang"; ?>" />
  <meta name="author" content="Tomasz Bojczuk" />
  <meta http-equiv="Reply-to" content="seelook@gmail.com" />
  <meta name="robots" content="all" />
  <link rel="stylesheet" type="text/css" href="style.css" media="screen" />
</head>

<body>
  <div id="menu">
    <a href="index.php?C=about">About</a><br />
    <a href="">News</a><br />
    <a href="index.php?C=scr">Screenshots</a><br />
    <a href="index.php?C=down">Downloads</a><br />
    <a href="https://sourceforge.net/projects/nootka/">Sourceforge</a>
  </div>
  <div id="main">
    <div id="head">
<!--       <img style="width: 100%; height: 40px;" src="" alt="uncommited image" /> -->
      Nootka<br />
      <span style="font-size: 50%;">...to play from scores</span>
    </div>
    <br />
    <div id="news">
      03.04.2011 - First public release is done.
    </div>
    <div id="cont">
      <br /><br />
      <? include($Contain.".php") ?>
      <br /><br />
    </div>
  </div>

</body>
</html>
