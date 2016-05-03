<?php
include("SpellCorrector.php");
//include("suggestData.php");
//ini_set('display_errors', 1);
//ini_set('display_startup_errors', 1);
//error_reporting(E_ALL);

header('Content-type: application/json');
header('Access-Control-Allow-Origin: *');
header('Access-Control-Allow-Methods: GET, POST, OPTIONS');
header('Access-Control-Allow-Headers: Origin, Content-Type, Accept, Authorization, X-Request-With');
header('Access-Control-Allow-Credentials: true');
// make sure browsers see this page as utf-8 encoded HTML
header('Content-Type: text/html; charset=utf-8');

$limit = 10;
$query = isset($_REQUEST['q']) ? $_REQUEST['q'] : false;
$results = false;
$new_query="";
$flag=0;
$final_query="";
//"http://localhost:8983/solr/mycore3/suggest?wt=json&indent=true&q="
if ($query)
{
  $url="http://localhost:8983/solr/mycore3/suggest?wt=json&indent=true&q=".$query;
  $string=file_get_contents($url);
  $json_res=json_decode($string,true); 
  $words= $json_res['suggest']['suggest'][$query]["suggestions"];
  $term1=$words[0]['term'];
  
  


   //echo $words['term'];

  $query_terms=explode(" ", $query);
 // $new_query=$query;//new query set to input query.It will change if misspelled words are der in a query

  foreach ($query_terms as $term) {
    $result=SpellCorrector::correct($term);
    if(strcmp($result,$term)){
      if(empty($new_query))
        $new_query=$result;
      else{
        $new_query=$new_query." ".$result;
      }
      
      $flag=1;
    }
    else{
      if(empty($new_query))
        $new_query=$result;
      else{
        $new_query=$new_query." ".$result;
      }
    }

    if($flag==1){
      $final_query=$new_query;
    }
    else{
      $final_query=$query;
    }
  }
 



  // The Apache Solr Client library should be on the include path
  // which is usually most easily accomplished by placing in the
  // same directory as this script ( . or current directory is a default
  // php include path entry in the php.ini)
  require_once('solr-php-client/Apache/Solr/Service.php');

  // create a new solr service instance - host, port, and webapp
  // path (all defaults in this example)
  $solr = new Apache_Solr_Service('localhost', 8983, 'solr/mycore3');

  // if magic quotes is enabled then stripslashes will be needed
  if (get_magic_quotes_gpc() == 1)
  {
    $query = stripslashes($query);
  }

  // in production code you'll always want to use a try /catch for any
  // possible exceptions emitted  by searching (i.e. connection
  // problems or a query parsing error)
  $additionalParameters=array('sort' => 'pageRankFile desc' );
   
   $rank=0;
   $autoCompleteParam = array();
   $autoCompleteParam['qt'] = '/suggest';
   
if(isset($_REQUEST['pagerank']) && $_REQUEST['pagerank']=="pagerankurl"){
	
  	try{

      $rank=1;
    	$results = $solr->search($query, 0, $limit, $additionalParameters);
     // $suggest_res=$solr->search($query,0,$limit,$autoCompleteParam);
      //$res = $solr->
      //print_r($results);
     
     
	}
	catch (Exception $e){
    
    	// in production you'd probably log or email this error to an admin
    	// and then show a special message to the user but for this example
    	// we're going to show the full exception
    	die("<html><head><title>SEARCH EXCEPTION</title><body><pre>{$e->__toString()}</pre></body></html>");
  	}
} 
else {
  	try{
    	$results = $solr->search($query, 0, $limit);
	}
	catch (Exception $e){
    	// in production you'd probably log or email this error to an admin
    	// and then show a special message to the user but for this example
    	// we're going to show the full exception
    	die("<html><head><title>SEARCH EXCEPTION</title><body><pre>{$e->__toString()}</pre></body></html>");
	}
   }
  }//if query ends

 $fileData=fopen('/home/vrushali/Desktop/InformationRetrieval/pageRankMapData.csv','r');
 $urlMap = array();
while($row=fgets($fileData)){ 
      $cols = explode(",", $row); 
      $urlMap[trim($cols[1])] = trim($cols[0]);
    }

?>
<html>
  <head>

    <script src="https://ajax.googleapis.com/ajax/libs/jquery/1.12.0/jquery.min.js"></script>
    <script src="//code.jquery.com/ui/1.11.4/jquery-ui.js"></script>
    <link rel="stylesheet" href="//code.jquery.com/ui/1.11.4/themes/smoothness/jquery-ui.css">
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap.min.css">
    <link rel="stylesheet" href="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/css/bootstrap-theme.min.css">
    <script src="https://maxcdn.bootstrapcdn.com/bootstrap/3.3.5/js/bootstrap.min.js"></script>
   
    <script>
$(document).ready(function(){
   var storedQuery = "";
   var suggQue = "";
   //$("#q").keydown(function(){
//  $("#q").css("background-color", "yellow");
 //  });
   $("#q").keyup(function(e){
var que = $.trim($("#q").val());
if(e.keyCode == 32){
      // user has pressed space
      storedQuery = $.trim($("#q").val()) + " ";
console.log("stored query is " + storedQuery);
$("#q").autocomplete({
source: [storedQuery]
});
}
else if (e.keyCode == 8){
var tokArr = que.split(" ");
suggQue = $.trim(tokArr[tokArr.length-1]);


storedQuery = (tokArr.slice(0, tokArr.length-1)).join(" ") + " ";

$.ajax({
   url: './suggestData.php',
   type: 'GET',
   data: {"q" : suggQue, "storedQue" : storedQuery}, //previous data passed to append the results
   success: function(response) { 
console.log(response);  
$("#q").autocomplete({
source: response
});
    }
});

}
else {
suggQue = que.replace(storedQuery, "");
//console.log("print the suggwuery ");  
//console.log(suggQue);

$.ajax({
   url: './suggestData.php',
   type: 'GET',
   data: {"q" : suggQue, "storedQue" : storedQuery},
   success: function(response) { 
console.log(response);  
$("#q").autocomplete({
source: response
});
    }
});
}
//$("#q").css("background-color", "pink");
   });
});
</script>

<title>PHP Solr Client Example</title>
  </head>
  <body>
    <div class="search">
      <div class="form">
        <form  accept-charset="utf-8" method="get">
          <label for="q">Search:</label>
          <input id="q" name="q" autocomplete="off" type="text" value="<?php echo htmlspecialchars($query, ENT_QUOTES, 'utf-8'); ?>"/>
          <input type="submit"/><br>
          <input type="radio" name="pagerank" value="pagerankurl"<?php if(isset($_REQUEST['pagerank']) && $_REQUEST['pagerank']=="pagerankurl"){ echo 'checked ="checked"';}?> >Search Using PageRank
          <input type="radio" name="pagerank" value="solr"<?php if(isset($_REQUEST['pagerank']) && $_REQUEST['pagerank']=="solr"){ echo 'checked ="checked"';}?> >Search using Solr<br>
        </form>
      </div>
<?php

// display results
 if($flag==1)
  {
    //$query=$new_query;
    /*$print_msg= "showing results for ".$new_query;
    $print_msg=$print_msg."<br>"."Search instead of ".$query;*/
     if($rank==1){//use pagerank
      echo "Did you mean :<I><b><a href='http://localhost/index.php?q=$new_query&pagerank=pagerankurl'>$new_query</a></b></I>";
     // echo "Showing results for <I><b><a href='http://localhost/index.php?q=$new_query&pagerank=pagerankurl'>$new_query</a></b></I>";
     // echo "<br>Search instead for  <a href='http://localhost/index.php?q=$query&pagerank=pagerankurl'>$query</a>";
     // $flag=0;
    }
    else{//use solr degault pagerank
      echo "Did you mean :<I><b><a href='http://localhost/index.php?q=$new_query&pagerank=solr'>$new_query</a></b></I>";

      //echo "Showing results for <I><b><a href='http://localhost/index.php?q=$new_query&pagerank=solr'>$new_query</a></b></I>";
      //echo "<br>Search instead for <a href='http://localhost/index.php?q=$query&pagerank=solr'>$query</a>";
      //$flag=0;
    }

}


if ($results)
{
  $total = (int) $results->response->numFound;
  $start = min(1, $total);
  $end = min($limit, $total);
?>
    <div>Results <?php echo $start; ?> - <?php echo $end;?> of <?php echo $total; ?>:</div>
    <hr>

<div class="result">
    <ol>
<?php
  // iterate result documents
  foreach ($results->response->docs as $doc)
  {
?>
    <li>
        <table width="1000" style="border: 1px solid black; text-align: left">
        <tr>
            <td><b>Title</b></td>
            <td><?php echo $doc->title; ?></td>
          </tr>
          <tr>
            <td><b>Author</b></td>
            <?php if($doc->author) { ?>
            <td><?php echo $doc->author; ?></td>
      <?php } else { ?>
            <td>N/A</td>
      <?php } ?>
          </tr>
          <tr>
            <td><b>Date created</b></td>
      <?php if($doc->created) { ?>
            <td><?php echo $doc->created; ?></td>
      <?php } else { ?>
            <td>N/A</td>
      <?php } ?>
          </tr>
    <tr>
             <td><b>Size</b></td>
             <td><?php echo ($doc->stream_size)/1024; ?>KB</td>
          </tr>
     <tr>
             <td colspan="2">
              <a href="<?php echo  $urlMap[trim($doc->id)]?>">Link</a>
             </td>
     </tr>
        </table>
      </li>
   
<?php
  }
?>
    </ol>
     </div>
  </div>
<?php
}
?>
  </body>
</html>