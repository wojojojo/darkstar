-- MySQL dump 10.13  Distrib 5.7.10, for Win64 (x86_64)
--
-- Host: localhost    Database: dspdb
-- ------------------------------------------------------
-- Server version	5.7.10-log

/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;
/*!40103 SET @OLD_TIME_ZONE=@@TIME_ZONE */;
/*!40103 SET TIME_ZONE='+00:00' */;
/*!40014 SET @OLD_UNIQUE_CHECKS=@@UNIQUE_CHECKS, UNIQUE_CHECKS=0 */;
/*!40014 SET @OLD_FOREIGN_KEY_CHECKS=@@FOREIGN_KEY_CHECKS, FOREIGN_KEY_CHECKS=0 */;
/*!40101 SET @OLD_SQL_MODE=@@SQL_MODE, SQL_MODE='NO_AUTO_VALUE_ON_ZERO' */;
/*!40111 SET @OLD_SQL_NOTES=@@SQL_NOTES, SQL_NOTES=0 */;

--
-- Table structure for table `char_exp`
--

DROP TABLE IF EXISTS `char_exp`;
/*!40101 SET @saved_cs_client     = @@character_set_client */;
/*!40101 SET character_set_client = utf8 */;
CREATE TABLE `char_exp` (
  `charid` int(10) unsigned NOT NULL,
  `mode` tinyint(1) unsigned NOT NULL DEFAULT '0',
  `war` smallint(5) unsigned NOT NULL DEFAULT '0',
  `mnk` smallint(5) unsigned NOT NULL DEFAULT '0',
  `whm` smallint(5) unsigned NOT NULL DEFAULT '0',
  `blm` smallint(5) unsigned NOT NULL DEFAULT '0',
  `rdm` smallint(5) unsigned NOT NULL DEFAULT '0',
  `thf` smallint(5) unsigned NOT NULL DEFAULT '0',
  `pld` smallint(5) unsigned NOT NULL DEFAULT '0',
  `drk` smallint(5) unsigned NOT NULL DEFAULT '0',
  `bst` smallint(5) unsigned NOT NULL DEFAULT '0',
  `brd` smallint(5) unsigned NOT NULL DEFAULT '0',
  `rng` smallint(5) unsigned NOT NULL DEFAULT '0',
  `sam` smallint(5) unsigned NOT NULL DEFAULT '0',
  `nin` smallint(5) unsigned NOT NULL DEFAULT '0',
  `drg` smallint(5) unsigned NOT NULL DEFAULT '0',
  `smn` smallint(5) unsigned NOT NULL DEFAULT '0',
  `blu` smallint(5) unsigned NOT NULL DEFAULT '0',
  `cor` smallint(5) unsigned NOT NULL DEFAULT '0',
  `pup` smallint(5) unsigned NOT NULL DEFAULT '0',
  `dnc` smallint(5) unsigned NOT NULL DEFAULT '0',
  `sch` smallint(5) unsigned NOT NULL DEFAULT '0',
  `geo` smallint(5) unsigned NOT NULL DEFAULT '0',
  `run` smallint(5) unsigned NOT NULL DEFAULT '0',
  `merits` tinyint(2) unsigned NOT NULL DEFAULT '0',
  `limits` smallint(5) unsigned NOT NULL DEFAULT '0',
  PRIMARY KEY (`charid`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8 AVG_ROW_LENGTH=85;
/*!40101 SET character_set_client = @saved_cs_client */;

--
-- Dumping data for table `char_exp`
--

LOCK TABLES `char_exp` WRITE;
/*!40000 ALTER TABLE `char_exp` DISABLE KEYS */;
INSERT INTO `char_exp` VALUES (21828,0,0,0,0,0,0,0,0,0,0,0,0,0,6499,0,0,0,0,0,0,0,0,0,0,0),(21829,0,0,0,6499,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
/*!40000 ALTER TABLE `char_exp` ENABLE KEYS */;
UNLOCK TABLES;
/*!40103 SET TIME_ZONE=@OLD_TIME_ZONE */;

/*!40101 SET SQL_MODE=@OLD_SQL_MODE */;
/*!40014 SET FOREIGN_KEY_CHECKS=@OLD_FOREIGN_KEY_CHECKS */;
/*!40014 SET UNIQUE_CHECKS=@OLD_UNIQUE_CHECKS */;
/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
/*!40111 SET SQL_NOTES=@OLD_SQL_NOTES */;

-- Dump completed on 2016-02-28  8:11:34
