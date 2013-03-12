<?php

class ModelAccountingAccounting extends Model
{

    public function run_counters( $start=NULL, $end = NULL )
    {	
        $now = time();
        $counter = array();
        $accepteddomains = array_flip( $this->__getAcceptedDomains() );
        $return = array(
            'starttimestamp' => 0,
            'endtimestamp' => 0,
            'addedstats' => 0,
            'deletedstats' => 0,
            );
        
        if ( !is_null($start) )
        {
            $start = $this->__decodeDate( $start );       
        } elseif ( is_null($start) )
        {
            //if we are passed nothing, operate on today
            $start = $this->__decodeDate( "00:00:00" );
        }

        if ( !is_null($end) )
        {
            $end = $this->__decodeDate( $end );
            $end = $end + 86400;            
        } elseif ( is_null($end) )
        {
            //if we are passed nothing, operate on today
            $end = $this->__decodeDate( "00:00:00" );
            $end = $end + 86400;
        }
        
        $return['starttimestamp'] = $start;
        $return['endtimestamp'] = $end;
        
        // run query to return all messages
        $tousers = $this->db->query('SELECT `sent`-(`sent`%86400) as `day`,`to`,count(*) as `count`,sum(`size`) as `size` FROM ' . VIEW_MESSAGES . ' WHERE `sent` >= '.$start.' AND `sent` < '.$end.' GROUP BY FROM_UNIXTIME(`day`, "%Y.%m.%d."), `to`;');
        $fromusers = $this->db->query('SELECT `sent`-(`sent`%86400) as `day`,`from`,count(*) as `count`,sum(`size`) as `size` FROM ' . VIEW_MESSAGES . ' WHERE `sent` >= '.$start.' AND `sent` < '.$end.' GROUP BY FROM_UNIXTIME(`day`, "%Y.%m.%d."), `from`;');
        
        // process results from above four queries
        if($tousers->num_rows > 0)
        {
            foreach($tousers->rows as $row)
            {
                $counter[$row['day']][$row['to']]['recd'] = $row['count'];
                $counter[$row['day']][$row['to']]['sizerecd'] = $row['size'];
            }
        }
        if($fromusers->num_rows > 0)
        {
            foreach($fromusers->rows as $row)
            {
                $counter[$row['day']][$row['from']]['sent'] = $row['count'];
                $counter[$row['day']][$row['from']]['sizesent'] = $row['size'];
            }
        }
        
        foreach( $counter as $date=>$users )
        {
        
            // execute queries to update the users and domains counter table
            $deletestats = $this->db->query("DELETE FROM " . TABLE_STAT_COUNTER . " WHERE `date` = $date;");
            $return['deletedstats'] = $return['deletedstats'] + $this->db->countAffected();
            
            foreach( $users as $username => $userdata)
            {
                //todo: consolidate
                $sent = isset($userdata['sent']) ? $userdata['sent'] : 0;
                $recd = isset($userdata['recd']) ? $userdata['recd'] : 0;
                $sizesent = isset($userdata['sizesent']) ? $userdata['sizesent'] : 0;
                $sizerecd = isset($userdata['sizerecd']) ? $userdata['sizerecd'] : 0;
                
                $parts = explode('@',$username);
                
                if ( isset($accepteddomains[ $parts[1] ]) ) {
                   $addusers = $this->db->query("INSERT INTO " . TABLE_STAT_COUNTER . " (`date`,`email`,`domain`,`sent`,`recd`,`sentsize`,`recdsize`) VALUES($date,'$username','".$parts[1]."',$sent,$recd,$sizesent,$sizerecd);");
                   $return['addedstats'] = $return['addedstats'] + $this->db->countAffected();
                }
            }
        
        }
        
		if(ENABLE_SYSLOG == 1) { syslog(LOG_INFO, sprintf("processed %s to %s: %d records deleted, %d records added",date("d M Y",$return['starttimestamp']),date("d M Y",$return['endtimestamp']),$return['deletedstats'],$return['addedstats'])); }
		
        return $return;
    }

    public function get_accounting($item = 'email',$search='',$page=0,$pagelen=0,$sort='item',$order=0 ) {

        // item can be either email or domain, maybe folder in the future??
    
        $_order = 0;
        $_order = "";
        $limit = "";
        $return = array();
    
        /*
        if(MEMCACHED_ENABLED) {
         $memcache = Registry::get('memcache');

         $statscounter = $memcache->get(Registry::get('statscounters'));

         if(isset($counter[MEMCACHED_PREFIX . 'counters_last_update'])) {
            if(isset($counter[MEMCACHED_PREFIX . 'size'])) { $asize = nice_size($counter[MEMCACHED_PREFIX . 'size'], ' '); }
            unset($counter[MEMCACHED_PREFIX . 'size']);

            return array ($asize, $counter);
         }
        } */
    
        $account_for_emails = $this->__getEmails();
        $account_for_domains = $this->__getDomains();
        
        $search = preg_replace("/\s{1,}/", "", $search);
                
        if ($item == 'email') {
            $account_for_emails = $this->__getEmails();
            $account_for_domains = $this->__getDomains();
            $query = "SELECT `email` AS `item`,MIN(`date`) as `oldest`,MAX(`date`) as `newest`,sum(`sent`) as `sent`,sum(`recd`) as `recd`,SUM(`sentsize`) as `sentsize`,AVG(`sentsize`) as `sentavg`,SUM(`recdsize`) as `recdsize`,AVG(`recdsize`) as `recdavg` FROM " . TABLE_STAT_COUNTER;
            $where = "WHERE ( `email` IN ('".implode("','",$account_for_emails)."') OR `domain` IN ('".implode("','",$account_for_domains)."') )";
            if($search){
                $where .= " AND ( `email` like '%".$search."%' OR `domain` like '%".$search."%' )";
            }
            $group = "GROUP BY `email`";
        } elseif ($item == 'domain') {
            $account_for_domains = $this->__getDomains();
            $query = "SELECT `domain` AS `item`,MIN(`date`) as `oldest`,MAX(`date`) as `newest`,sum(`sent`) as `sent`,sum(`recd`) as `recd`,SUM(`sentsize`) as `sentsize`,AVG(`sentsize`) as `sentavg`,SUM(`recdsize`) as `recdsize`,AVG(`recdsize`) as `recdavg` FROM " . TABLE_STAT_COUNTER;
            $where = "WHERE ( `domain` IN ('".implode("','",$account_for_domains)."') )";
            if($search){
                $where .= " AND `domain` like '%".$search."%'";
            }            
            $group = "GROUP BY `domain`";
        } else {
            return false;
        }

        if($order == 0) { $order = "ASC"; }
        else { $order = "DESC"; }
      
        $_order = "ORDER BY `$sort` $order";
        
        $from = (int)$page * (int)$pagelen;
        
        if($pagelen > 0) { $limit = " LIMIT " . (int)$from . ", " . (int)$pagelen; }
        
        $query = $this->db->query($query.' '.$where.' '.$group.' '.$_order.' '.$limit.';');

        if($query->num_rows >= 1)
        {
            return $query->rows;
        } else {
            // no results found
            return false;
        }
    }

    
    public function count_accounting($item = 'email',$search='') {
    
        $account_for_emails = $this->__getEmails();
        $account_for_domains = $this->__getDomains();
        
        $search = preg_replace("/\s{1,}/", "", $search);
        
        if($search){
            $search_cond .= " AND ( `email` like '%".$search."%' OR `domain` like '%".$search."%' )";
        }
        
        $query = "SELECT `email` AS `item`,MIN(`date`) as `oldest`,MAX(`date`) as `newest`,sum(`sent`) as `sent`,sum(`recd`) as `recd`,sum(`sentsize`) as `sentsize`,sum(`recdsize`) as `recdsize` FROM " . TABLE_STAT_COUNTER;
        
        if ($item == 'email') {
            $where = "WHERE `email` IN ('".implode("','",$account_for_emails)."') OR `domain` IN ('".implode("','",$account_for_domains)."')";
            if($search){
                $where .= " AND ( `email` like '%".$search."%' OR `domain` like '%".$search."%' )";
            }
            $group = "GROUP BY `email`";
        } elseif ($item == 'domain') {
            $where = "WHERE `domain` IN ('".implode("','",$account_for_domains)."')";
            if($search){
                $where .= " AND `domain` like '%".$search."%'";
            }            
            $group = "GROUP BY `domain`";
        } else {
            return false;
        }

        $query = $this->db->query($query.' '.$where.' '.$group.';');

        return $query->num_rows;
    }
    
    private function __getEmails() {
        $return = array();
        array_push($return,$_SESSION['email']);
        foreach ($_SESSION['emails'] as $e) {
            array_push($return,$e);
        }
        
        return $return;
    }   
    
    private function __getDomains() {
        $return = array();
        
        if(Registry::get('admin_user') >= 1) {
            $return = $this->__getAcceptedDomains();
        }elseif(Registry::get('auditor_user') == 1) {
            array_push($return,$_SESSION['domain']);
            foreach ($_SESSION['auditdomains'] as $d) {
                array_push($return,$d);
            }
        }
        
        return $return;
    }  
    
    
    private function __getAcceptedDomains() {
        // todo: move to domains model?
        $return = array();
        
        $query = $this->db->query("SELECT domain, mapped FROM " . TABLE_DOMAIN . " ORDER BY domain ASC");
        
        foreach($query->rows as $domain) 
        {
            if ($domain['domain'] == $domain['mapped']) {
                array_push($return,$domain['domain']);
            } else {
                array_push($return,$domain['mapped']);
            }
        }
        
        return $return;
    }
     
    private function __decodeDate( $input ) {
        
        if ( !is_numeric($input) )
        {
            // if we are passed words (yesterday, today), convert to unix timestamps representing the start of the day
            $input = strtotime($input);
            $return = $input - ($input%86400);
        
        } elseif ( is_numeric($input) )
        {
            // if we are passed unix timestamps, ensure they represent the start of the day
            $return = $input - ($input%86400);
        }
        
        return $return;
        
    }

}
?>
