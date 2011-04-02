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
</head>
<body>
  <div id="menu">
    <a href="">About</a>
    <a href="">News</a>
    <a href="">Screenshots</a>
    <a href="">Downloads</a>
  </div>
  <div id="main">
    <div id="head">
    </div>
    <div id="news">
    </div>
    <div id="cont">
    </div>
  </div>

</body>
</html>
