SET SQL_MODE="NO_AUTO_VALUE_ON_ZERO";
SET time_zone = "+00:00";


/*!40101 SET @OLD_CHARACTER_SET_CLIENT=@@CHARACTER_SET_CLIENT */;
/*!40101 SET @OLD_CHARACTER_SET_RESULTS=@@CHARACTER_SET_RESULTS */;
/*!40101 SET @OLD_COLLATION_CONNECTION=@@COLLATION_CONNECTION */;
/*!40101 SET NAMES utf8 */;

--
-- database: `sa`
--
CREATE DATABASE IF NOT EXISTS sa;
USE sa;

-- --------------------------------------------------------

--
-- Table structure `capturepet`
--

CREATE TABLE IF NOT EXISTS `capturepet` (
  `unicode` varchar(32) NOT NULL,
  `id` int(11) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `type` int(11) NOT NULL,
  `lv` int(11) NOT NULL,
  `hp` int(11) NOT NULL,
  `attack` int(11) NOT NULL,
  `def` int(11) NOT NULL,
  `quick` int(11) NOT NULL,
  `sum` double NOT NULL,
  `author` varchar(32) CHARACTER SET utf8 NOT NULL,
  `cdkey` varchar(32) CHARACTER SET utf8 NOT NULL,
  `check` tinyint(1) NOT NULL DEFAULT '0',
  `inserttime` datetime NOT NULL,
  PRIMARY KEY (`unicode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `CashDraw`
--

CREATE TABLE IF NOT EXISTS `CashDraw` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `type` varchar(16) NOT NULL,
  `point` int(11) NOT NULL,
  `fixtime` datetime NOT NULL,
  PRIMARY KEY (`id`),
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `check`
--

CREATE TABLE IF NOT EXISTS `check` (
  `itemcode` varchar(64) NOT NULL,
  `cdkey` varchar(32) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `mac` varchar(64) NOT NULL,
  `type` varchar(32) CHARACTER SET utf8 NOT NULL,
  `value` int(11) NOT NULL,
  `time` datetime NOT NULL,
  `check` tinyint(4) NOT NULL DEFAULT '0',
  `usecdkey` varchar(32) DEFAULT NULL,
  `usename` varchar(32) CHARACTER SET utf8 DEFAULT NULL,
  `usemac` varchar(64) DEFAULT NULL,
  `usetime` datetime DEFAULT NULL,
  PRIMARY KEY (`itemcode`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `consignment`
--

CREATE TABLE IF NOT EXISTS `consignment` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) COLLATE utf8_general_ci NOT NULL,
  `name` varchar(32) COLLATE utf8_general_ci NOT NULL,
  `alive` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `time` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

-- --------------------------------------------------------

--
-- Table structure `counter`
--

CREATE TABLE IF NOT EXISTS `counter` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `type` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `alive` int(11) NOT NULL,
  `fame` int(11) NOT NULL,
  `fixtime` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `CSAinfo`
--

CREATE TABLE IF NOT EXISTS `CSAinfo` (
  `cdkey` varchar(32) CHARACTER SET utf8 NOT NULL,
  `saveindex` int(11) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `famename` varchar(32) CHARACTER SET utf8 COLLATE utf8_general_ci NOT NULL,
  `CE` int(16) NOT NULL DEFAULT '0',
  `level` int(11) NOT NULL,
  `trans` int(11) NOT NULL,
  `alive` int(11) NOT NULL,
  `cameo` int(11) NOT NULL,
  `fame` int(11) NOT NULL,
  `TITLE_LV` int(11) NOT NULL,
  `DUELBATTLECOUNT` int(11) NOT NULL,
  `DUELWINCOUNT` int(11) NOT NULL,
  `DUELLOSECOUNT` int(11) NOT NULL,
  `DUELSTWINCOUNT` int(11) NOT NULL,
  `DUELSTLOSECOUNT` int(11) NOT NULL,
  `DUELMAXSTWINCOUNT` int(11) NOT NULL,
  `RESETPETCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Washing',
  `AWAKEPETCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Variation',
  `SEALEVNTCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Task seal number',
  `TRANSPETCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Pet',
  `KILLBOSSCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Kill BOSSfrequency',
  `BOSSKILLCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'quiltBOSSKill',
  `CAPPETCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Gratify the number of pets',
  `FEEDMMCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'practiceMMfrequency',
  `EQUIPUPCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Strengthen the number of success',
  `EQUIPFAILCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Strengthen failure',
  `MAGICADDCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Consigne',
  `HORSEWINCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Mid-range',
  `HUNTFISHCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Fishing',
  `HUNTBIGFISHCOUNT` int(16) NOT NULL DEFAULT '0' COMMENT 'Big fish',
  `vip` int(11) NOT NULL,
  `time` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

-- --------------------------------------------------------

--
-- Table structure `CSA_Login`
--

CREATE TABLE IF NOT EXISTS `CSA_Login` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `Name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `PassWord` varchar(32) CHARACTER SET utf8 NOT NULL,
  `IP` varchar(16) NOT NULL DEFAULT '-',
  `MAC` varchar(64) NOT NULL,
  `RegTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `LoginTime` datetime DEFAULT '0000-00-00 00:00:00',
  `OnlineName` varchar(32) CHARACTER SET utf8 DEFAULT NULL,
  `OnlineName1` varchar(30) CHARACTER SET utf8 DEFAULT NULL,
  `OnlineName2` varchar(32) CHARACTER SET utf8 DEFAULT NULL,
  `Online` tinyint(4) DEFAULT '0',
  `Path` varchar(16) DEFAULT '',
  `CE` int(16) NOT NULL DEFAULT '0',
  `VipPoint` int(11) DEFAULT '0',
  `PayPoint` int(11) NOT NULL,
  `GamblePoint` int(11) NOT NULL,
  `TrialPoint` int(16) NOT NULL DEFAULT '0',
  `GoldPoint` int(16) NOT NULL DEFAULT '0',
  `SQLPoint` int(16) NOT NULL DEFAULT '0',
  `SafePasswd` varchar(32) DEFAULT '',
  `ServerId` int(4) NOT NULL DEFAULT '0',
  `Regip` varchar(32) NOT NULL,
  `qq` varchar(32) NOT NULL,
  `tel` varchar(32) NOT NULL,
  `ServerName` varchar(32) NOT NULL,
  `Recommender` int(11) NOT NULL,
  `Lock` int(4) NOT NULL DEFAULT '0',
  `LockTime` datetime NOT NULL DEFAULT '0000-00-00 00:00:00',
  `LockInfo` varchar(32) NOT NULL DEFAULT '',
  `Offline` tinyint(4) NOT NULL DEFAULT '0',
  `Trust` tinyint(4) NOT NULL DEFAULT '0',
  PRIMARY KEY (`id`),
  UNIQUE KEY `Name` (`Name`)
) ENGINE=MyISAM  DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `dice`
--

CREATE TABLE IF NOT EXISTS `dice` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `type` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `alive` int(11) NOT NULL,
  `fame` int(11) NOT NULL,
  `dicenum` int(11) NOT NULL,
  `fixtime` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `dicelog`
--

CREATE TABLE IF NOT EXISTS `dicelog` (
  `type` int(11) NOT NULL DEFAULT '0',
  `winpoint` int(11) NOT NULL DEFAULT '0',
  `losepoint` int(11) NOT NULL DEFAULT '0',
  PRIMARY KEY (`type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `loan`
--

CREATE TABLE IF NOT EXISTS `loan` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) CHARACTER SET utf8 NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `PetName` varchar(32) CHARACTER SET utf8 NOT NULL,
  `PetInfo` varchar(512) CHARACTER SET utf8 NOT NULL,
  `PetString` varchar(2048) CHARACTER SET utf8 NOT NULL,
  `day` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `stat` int(11) NOT NULL DEFAULT '0',
  `loancdkey` varchar(32) CHARACTER SET utf8 NOT NULL,
  `loanname` varchar(32) CHARACTER SET utf8 NOT NULL,
  `fixtime` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `LOCK`
--

CREATE TABLE IF NOT EXISTS `LOCK` (
  `Name` varchar(64) NOT NULL,
  `LockTime` datetime NOT NULL,
  PRIMARY KEY (`Name`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `OnlineBuy`
--

CREATE TABLE IF NOT EXISTS `OnlineBuy` (
  `CostPasswd` varchar(32) NOT NULL,
  `CostStr` varchar(64) DEFAULT NULL,
  `cdkey` varchar(32) DEFAULT NULL,
  `CostTime` datetime DEFAULT NULL,
  `check` tinyint(1) DEFAULT '1',
  PRIMARY KEY (`CostPasswd`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `OnlineCard`
--

CREATE TABLE IF NOT EXISTS `OnlineCard` (
  `CostPasswd` varchar(32) NOT NULL,
  `CostStr` varchar(64) DEFAULT NULL,
  `cdkey` varchar(32) DEFAULT NULL,
  `CostTime` datetime DEFAULT NULL,
  `check` tinyint(1) DEFAULT '1',
  PRIMARY KEY (`CostPasswd`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `OnlineCost`
--

CREATE TABLE IF NOT EXISTS `OnlineCost` (
  `CostPasswd` varchar(32) NOT NULL,
  `CostVal` int(11) NOT NULL,
  `PayVal` int(11) NOT NULL,
  `cdkey` varchar(32) DEFAULT '',
  `CostTime` datetime NOT NULL,
  `check` tinyint(1) NOT NULL DEFAULT '1',
  `creator` varchar(32) DEFAULT NULL,
  UNIQUE KEY `CostPasswd` (`CostPasswd`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `OnlinePlayer`
--

CREATE TABLE IF NOT EXISTS `OnlinePlayer` (
  `cdkey` varchar(32) NOT NULL,
  `MAC` varchar(64) NOT NULL,
  `Name` varchar(32) NOT NULL,
  `LoginTime` datetime NOT NULL,
  `ServerId` int(11) NOT NULL,
  PRIMARY KEY (`cdkey`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `PauctionInfo`
--

CREATE TABLE IF NOT EXISTS `PauctionInfo` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) CHARACTER SET utf8 NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `effect` varchar(1024) CHARACTER SET utf8 NOT NULL,
  `cost` int(11) NOT NULL,
  `type` int(11) NOT NULL,
  `info` varchar(1024) CHARACTER SET utf8 NOT NULL,
  `string` varchar(1024) CHARACTER SET utf8 NOT NULL,
  `day` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `petbilling`
--

CREATE TABLE IF NOT EXISTS `petbilling` (
  `id` int(11) NOT NULL,
  `type` tinyint(4) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `lv` int(11) NOT NULL,
  `hp` int(11) NOT NULL,
  `attack` int(11) NOT NULL,
  `def` int(11) NOT NULL,
  `quick` int(11) NOT NULL,
  `author` varchar(32) CHARACTER SET utf8 NOT NULL,
  PRIMARY KEY (`id`,`type`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `pklist`
--

CREATE TABLE IF NOT EXISTS `pklist` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `player1` varchar(32) CHARACTER SET utf8 NOT NULL,
  `player2` varchar(32) CHARACTER SET utf8 NOT NULL,
  `player3` varchar(32) CHARACTER SET utf8 NOT NULL,
  `player4` varchar(32) CHARACTER SET utf8 NOT NULL,
  `player5` varchar(32) CHARACTER SET utf8 NOT NULL,
  `seesion` int(11) NOT NULL,
  `stat` int(11) NOT NULL DEFAULT '0',
  UNIQUE KEY `id` (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `safedata`
--

CREATE TABLE IF NOT EXISTS `safedata` (
  `cdkey` varchar(32) NOT NULL DEFAULT '',
  `newsafe` varchar(64) DEFAULT NULL,
  `oldsafe` varchar(64) DEFAULT NULL,
  `flg` int(1) NOT NULL,
  `FixTime` datetime NOT NULL,
  PRIMARY KEY (`cdkey`)
) ENGINE=MyISAM DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `SaleVipPoint`
--

CREATE TABLE IF NOT EXISTS `SaleVipPoint` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `Vippoint` int(11) NOT NULL,
  `rate` int(11) NOT NULL,
  `price` int(11) NOT NULL,
  `stat` int(11) NOT NULL,
  `CreateTime` datetime NOT NULL,
  `PayTime` datetime NOT NULL,
  `banktype` varchar(32) CHARACTER SET utf8 NOT NULL DEFAULT '',
  `bankcdkey` varchar(32) CHARACTER SET utf8 NOT NULL,
  `bankname` varchar(8) CHARACTER SET utf8 NOT NULL,
  `QQ` varchar(11) CHARACTER SET utf8 NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `SuperStar`
--

CREATE TABLE IF NOT EXISTS `SuperStar` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) CHARACTER SET utf8 NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `award` varchar(32) CHARACTER SET utf8 NOT NULL,
  `check` int(11) NOT NULL,
  `datatime` datetime NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `transpointlog`
--

CREATE TABLE IF NOT EXISTS `transpointlog` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `incdkey` varchar(32) DEFAULT NULL,
  `outcdkey` varchar(32) DEFAULT NULL,
  `type` varchar(32) DEFAULT NULL,
  `inpoint` int(11) DEFAULT '0',
  `outpoint` int(11) DEFAULT '0',
  `datetime` datetime DEFAULT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `VipShop`
--

CREATE TABLE IF NOT EXISTS `VipShop` (
  `cdkey` varchar(32) COLLATE utf8_general_ci NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `itemid` int(11) NOT NULL,
  `itemname` varchar(32) CHARACTER SET utf8 NOT NULL,
  `itemnum` int(11) NOT NULL,
  `totalcost` int(16) NOT NULL,
  `time` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8 COLLATE=utf8_general_ci;

-- --------------------------------------------------------

--
-- Table structure `weekpay`
--

CREATE TABLE IF NOT EXISTS `weekpay` (
  `id` int(11) NOT NULL AUTO_INCREMENT,
  `term` int(11) NOT NULL,
  `cdkey` varchar(32) NOT NULL,
  `itemid` int(11) NOT NULL,
  `check` int(11) NOT NULL,
  PRIMARY KEY (`id`)
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `YamaKing`
--

CREATE TABLE IF NOT EXISTS `YamaKing` (
  `index` int(13) NOT NULL AUTO_INCREMENT,
  `cdkey` varchar(32) CHARACTER SET utf8 NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `floor` int(16) NOT NULL,
  `itemid` int(16) NOT NULL,
  `itemname` varchar(16) CHARACTER SET utf8 NOT NULL,
  `time` datetime NOT NULL,
  `mac` varchar(64) CHARACTER SET utf8 NOT NULL,
  `servername` varchar(32) CHARACTER SET utf8 NOT NULL,
  PRIMARY KEY (`index`)
) ENGINE=InnoDB  DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `yuxiaxie`
--

CREATE TABLE IF NOT EXISTS `yuxiaxie` (
  `date` date NOT NULL,
  `cdkey` varchar(32) CHARACTER SET utf8 NOT NULL,
  `name` varchar(32) CHARACTER SET utf8 NOT NULL,
  `type` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `fame` int(11) NOT NULL,
  `alive` int(11) NOT NULL,
  `fixtime` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

-- --------------------------------------------------------

--
-- Table structure `yuxiaxielog`
--

CREATE TABLE IF NOT EXISTS `yuxiaxielog` (
  `date` date NOT NULL,
  `type` int(11) NOT NULL,
  `vippoint` int(11) NOT NULL,
  `alive` int(11) NOT NULL,
  `fame` int(11) NOT NULL,
  `sumvippoint` int(11) NOT NULL,
  `sumalive` int(11) NOT NULL,
  `sumfame` int(11) NOT NULL,
  `fixtime` datetime NOT NULL
) ENGINE=InnoDB DEFAULT CHARSET=utf8;

/*!40101 SET CHARACTER_SET_CLIENT=@OLD_CHARACTER_SET_CLIENT */;
/*!40101 SET CHARACTER_SET_RESULTS=@OLD_CHARACTER_SET_RESULTS */;
/*!40101 SET COLLATION_CONNECTION=@OLD_COLLATION_CONNECTION */;
