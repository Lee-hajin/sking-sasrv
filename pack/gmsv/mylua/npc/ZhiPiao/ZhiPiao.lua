------------------------------- 杂类模块 -------------------------------
function StringSplit(_data, _splitint)--分割处理函数（返回数组）
	local StartIndex = 1;
	local SplitIndex = 1;
	local SplitArray = {};
	while true do
		local FindLastIndex = string.find(_data, _splitint, StartIndex);
		if not FindLastIndex then
			SplitArray[SplitIndex] = string.sub(_data, StartIndex, string.len(_data));
			break ;
		end
		SplitArray[SplitIndex] = string.sub(_data, StartIndex, FindLastIndex - 1);
		StartIndex = FindLastIndex + string.len(_splitint);
		SplitIndex = SplitIndex + 1;
	end
	return SplitArray;
end

function FreeZhiPiao(charaindex)
	local token = "1\n[style c=5]请选择要签发的支票类型：[/style]\n";
	for i=1,table.getn(TaxType) do
		token = token .. string.format("签发 [style c=4]%-s[/style] 支票([style c=6]税率：%-d%%[/style])\n", TaxType[i][1],TaxType[i][2]);
	end
	lssproto.windows(charaindex, 2, 8, 0, char.getWorkInt( npcindex, "对象"), token);
	--return 1;
end
------------------------------- 主NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	--if npc.isFaceToFace(_meindex, _talkerindex) ~= 1 then
		--return ;
	--end
	if (_select == 8) then
		return ;
	end
	if _seqno == 0 then
		local setid = other.atoi(_data);--获取选择选项
		local settype = TaxType[setid][1];--取得类型
		local setrate = TaxType[setid][2];--取得税率
		local mypoint = 0;
		if settype == "金币" then
			mypoint = sasql.getVipPoint(_talkerindex);
		elseif settype == "声望" then
			mypoint = math.floor(char.getInt(_talkerindex, "声望")/100);
		elseif settype == "活力" then
			mypoint = char.getInt(_talkerindex, "活力");
		elseif settype == "积分" then
			mypoint = char.getInt(_talkerindex, "积分");
		end
		local token = "您选择签发的支票类型为：[style c=4]"..settype.."[/style]\n"
					.."您当前的[style c=4]"..settype.."[/style]数量为："..mypoint
					.."\n签发该类型的支票税率为："..setrate.."%"
					.."\n\n\n\n请输入您要签发的[style c=4]"..settype.."[/style]数：";
		lssproto.windows(_talkerindex, 1, 12, 1, char.getWorkInt( _meindex, "对象"), token);
		
		char.setWorkInt(_talkerindex,"临时整数1",setid);--记录选择选项
	elseif _seqno == 1 then
		local setid = char.getWorkInt(_talkerindex,"临时整数1");--获取选择选项
		local settype = TaxType[setid][1];--取得类型
		local setrate = TaxType[setid][2];--取得税率
		local mypoint = 0;
		if settype == "金币" then
			mypoint = sasql.getVipPoint(_talkerindex);
		elseif settype == "声望" then
			mypoint = math.floor(char.getInt(_talkerindex, "声望")/100);
		elseif settype == "活力" then
			mypoint = char.getInt(_talkerindex, "活力");
		elseif settype == "积分" then
			mypoint = char.getInt(_talkerindex, "积分");
		end
		local setnum = other.atoi(_data);
		local ratenum = math.ceil(setnum / 100 * setrate);
		local token = "您选择签发的支票类型为：[style c=4]"..settype.."[/style]\n"
					.."您当前的[style c=4]"..settype.."[/style]数量为："..mypoint
					.."\n签发该类型的支票税率为："..setrate.."%"
					.."\n"
					.."\n签发该支票需要支付："
					.."\n       [style c=5]"..setnum.." + "..ratenum.." = "..(setnum+ratenum).." "..settype.."[/style]"
					.."\n\n[style c=6]你确定要签发该支票么？[/style]";
		lssproto.windows(_talkerindex, 0, 12, 2, char.getWorkInt( _meindex, "对象"), token);
		char.setWorkInt(_talkerindex,"临时整数1",setid);--记录选择选项
		char.setWorkInt(_talkerindex,"临时整数2",setnum);--记录选择选项
	elseif _seqno == 2 then
		local setid = char.getWorkInt(_talkerindex,"临时整数1");--获取选择选项
		local settype = TaxType[setid][1];--取得类型
		local setrate = TaxType[setid][2];--取得税率
		local mypoint = 0;
		if settype == "金币" then
			mypoint = sasql.getVipPoint(_talkerindex);
		elseif settype == "声望" then
			mypoint = math.floor(char.getInt(_talkerindex, "声望")/100);
		elseif settype == "活力" then
			mypoint = char.getInt(_talkerindex, "活力");
		elseif settype == "积分" then
			mypoint = char.getInt(_talkerindex, "积分");
		end
		local setnum = char.getWorkInt(_talkerindex,"临时整数2");
		local ratenum = math.ceil(setnum / 100 * setrate);
		if mypoint < (setnum+ratenum) then
			char.TalkToCli(_talkerindex, -1, "你的点数不足签发该支票！", 6);
			return ;
		end
		local itemindex = -1;
		local itemnum = char.GetEmptyItemNum(_talkerindex);--取得道具栏数量
		if itemnum < 1 then
			char.TalkToCli(_talkerindex, -1, "必须空出一个道具栏位！", 6);
			return ;
		end
		itemindex = char.Additem(_talkerindex,SetItem,1);
		local settime = os.time() + TaxTime;--签发时间
		local setname = char.getChar(_talkerindex,"名字");--签发帐号
		
		if settype == "金币" then
			sasql.setVipPoint(_talkerindex,mypoint-(setnum+ratenum));
		elseif settype == "声望" then
			char.setInt(_talkerindex, "声望",(mypoint - (setnum+ratenum))*100);
		elseif settype == "活力" then
			char.setInt(_talkerindex, "活力",mypoint-(setnum+ratenum));
		elseif settype == "积分" then
			char.setInt(_talkerindex, "积分",mypoint-(setnum+ratenum));
		end
		
		--将支票数值写进SQL
		item.setChar(itemindex,"名称",settype.."支票[".. setnum .."]");
		item.setChar(itemindex,"显示名",settype.."支票[".. setnum .."]");
		item.setChar(itemindex,"说明",setnum.."金币的支票，到期时间[".. os.date("%Y-%m-%d %X",settime) .."]签发人："..setname);
		--print(settype.."|"..setnum.."|"..settime);
		item.setChar(itemindex,"字段",settype.."|"..setnum.."|"..settime);
		char.sendStatusString(_talkerindex,"I");
		char.TalkToCli(_talkerindex, -1, "支票签发成功！", 4);
		--char.DelItem(charaindex, haveitemindex);--删除道具
	end
end

------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		local token = "1\n[style c=5]请选择要签发的支票类型：[/style]\n";
		for i=1,table.getn(TaxType) do
			token = token .. string.format("签发 [style c=4]%-s[/style] 支票([style c=6]税率：%-d%%[/style])\n", TaxType[i][1],TaxType[i][2]);
		end
		lssproto.windows(_talkerindex, 2, 8, 0, char.getWorkInt( _meindex, "对象"), token);
	end
end

------------------------------- 道具使用事件函数 -------------------------------
function ZhiPiaoItemCall(itemindex, charaindex, toindex, haveitemindex)
	if char.getInt(toindex,"类型") ~= 1 then
		char.TalkToCli(charaindex, -1, "该道具不可给宠物使用！", 6);
		return ;
	end
	local strdata = item.getChar(itemindex, "字段");--获取道具字段
	local strlist = StringSplit(strdata,"|");
	
	if table.getn(strlist) < 3 then
		char.TalkToCli(charaindex, -1, "该支票参数有误！", 6);
		return ;
	end
	local settype = strlist[1];--取得类型
	local setnum = other.atoi(strlist[2]);--取得数量
	local settime = other.atoi(strlist[3]);--取得时间
	if os.time() > settime then
		char.TalkToCli(charaindex, -1, "该支票已经到期，无法使用！", 6);
		return ;
	end
	
	local mypoint = 0;
	if settype == "金币" then
		mypoint = sasql.getVipPoint(charaindex);
	elseif settype == "声望" then
		mypoint = math.floor(char.getInt(charaindex, "声望")/100);
	elseif settype == "活力" then
		mypoint = char.getInt(charaindex, "活力");
	elseif settype == "积分" then
		mypoint = char.getInt(charaindex, "积分");
	end
	local setpoint = mypoint + setnum;
	if setpoint > 99999999 then
		char.TalkToCli(charaindex, -1, "使用后您的"..settype.."点数大于99999999，无法使用！", 6);
		return ;
	end
	if settype == "金币" then
		sasql.setVipPoint(charaindex,setpoint);
	elseif settype == "声望" then
		char.setInt(charaindex, "声望",setpoint*100);
	elseif settype == "活力" then
		char.setInt(charaindex, "活力",setpoint);
	elseif settype == "积分" then
		char.setInt(charaindex, "积分",setpoint);
	end
	char.TalkToCli(charaindex, -1, "获得"..setnum..settype, 4);
	char.DelItem(charaindex, haveitemindex);--删除道具
end

------------------------------- 主NPC创建事件函数 -------------------------------
function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

------------------------------- GM命令函数 -------------------------------
function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		mydata();
		char.setChar(npcindex, "名字",npcdata[1]);
		char.setInt(npcindex, "图像号", npcdata[2]);
		char.setInt(npcindex, "原图像号", npcdata[2]);
		char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
		char.setInt(npcindex, "方向", npcdata[6]);
		char.ToAroundChar(npcindex);
		char.TalkToCli(_charaindex, -1, "重读NPC["..char.getChar(npcindex, "名字").."]完成", 6);
	end
end

------------------------------- 常量设置函数 -------------------------------
function mydata()
	SetItem = 2257;--空头支票道具编号
	--itemset6.txt加入以下数据
	--空头支票,空头支票,使用后可签发支票的道具,,,,,,,,ITEM_ZhiPiao,,,,,,2257,24176,0,16,2,0,0,-1,,1,,,,,,,,,,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,,0,0,,,,,,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,1,1,0,0,,0,,0,,0,,0,,0,
	TaxTime = 60 * 60 * 24 * 15;
	--货币名,税率
	TaxType = {{"金币",2},
			   {"声望",18},
			   {"活力",2},
			   {"积分",2},
	          };--支票类型
	npcdata = {"支票签发员",100000,777,37,35,6,"ZhiPiao"};
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
	item.addLUAListFunction( "ITEM_ZhiPiao", "ZhiPiaoItemCall", "");
end
