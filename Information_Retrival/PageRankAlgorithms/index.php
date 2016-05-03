<?php

// make sure browsers see this page as utf-8 encoded HTML
header('Content-Type: text/html; charset=utf-8');

$limit = 10;
$query = isset($_REQUEST['q']) ? $_REQUEST['q'] : false;
$results = false;

if ($query)
{

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
if(isset($_REQUEST['pagerank']) && $_REQUEST['pagerank']=="pagerankurl"){
	
  	try{
    	$results = $solr->search($query, 0, $limit, $additionalParameters);
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
    <title>PHP Solr Client Example</title>
  </head>
  <body>
    <form  accept-charset="utf-8" method="get">
      <label for="q">Search:</label>
      <input id="q" name="q" type="text" value="<?php echo htmlspecialchars($query, ENT_QUOTES, 'utf-8'); ?>"/>
      <input type="submit"/><br>
      <input type="radio" name="pagerank" value="pagerankurl"<?php if(isset($_REQUEST['pagerank']) && $_REQUEST['pagerank']=="pagerankurl"){ echo 'checked ="checked"';}?> >Search Using PageRank
      <input type="radio" name="pagerank" value="solr"<?php if(isset($_REQUEST['pagerank']) && $_REQUEST['pagerank']=="solr"){ echo 'checked ="checked"';}?> >Search using Solr<br>
    </form>
<?php

// display results
if ($results)
{
  $total = (int) $results->response->numFound;
  $start = min(1, $total);
  $end = min($limit, $total);
?>
    <div>Results <?php echo $start; ?> - <?php echo $end;?> of <?php echo $total; ?>:</div>
    <ol>
<?php
  // iterate result documents
  foreach ($results->response->docs as $doc)
  {
?>
      <li>
        <table style="border: 1px solid black; text-align: left">
        <tr>
            <th>Title</th>
            <td><?php echo $doc->title; ?></td>
          </tr>
          <tr>
            <th>Author</th>
            <?php if($doc->author) { ?>
            <td><?php echo $doc->author; ?></td>
      <?php } else { ?>
            <td>N/A</td>
      <?php } ?>
          </tr>
          <tr>
            <th>Date created</th>
      <?php if($doc->created) { ?>
            <td><?php echo $doc->created; ?></td>
      <?php } else { ?>
            <td>N/A</td>
      <?php } ?>
          </tr>
    <tr>
             <th>Size</th>
             <td><?php echo ($doc->stream_size)/1024; ?>KB</td>
          </tr>
     <tr>
             <th colspan="2">
              <a href="<?php echo $urlMap[trim($doc->id)]?>"><?php echo $urlMap[trim($doc->id)]?></a>
             </th>
     </tr>
        </table>
      </li>
<?php
  }
?>
    </ol>
<?php
}
?>
  </body>
</html>