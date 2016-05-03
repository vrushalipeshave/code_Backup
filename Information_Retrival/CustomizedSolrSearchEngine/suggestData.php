
<?
include("PorterStemmer.php");
header('Content-type: application/json');
//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);

$storedStuff = $_GET['storedQue'];
if($_GET['q']){
	$quer=$_GET['q'];
	$call = "http://localhost:8983/solr/mycore3/suggest?&wt=json&q=" . $quer;
	
	$json=file_get_contents($call);
	
	$json_res=json_decode($json,true);
 	
    $words= $json_res['suggest']['suggest'][$quer]["suggestions"];
    $suggestions=array();
    $finalWeightKVPair=array();
  	
  	for($i=0;$i<count($words);$i++) {
	   $finalWeightKVPair[$words[$i]['term']] = $words[$i]['weight'];
	}
	
	arsort($finalWeightKVPair);//descending order of weights
    $count=0;
	foreach($finalWeightKVPair as $x => $x_value) {
    	$suggestions[$count]=$storedStuff.$x;
    	$count=$count+1;
	}

	print_r(json_encode($suggestions));
}

?>