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

function GetPreciseDecimal(nNum, n)
    if type(nNum) ~= "number" then
        return nNum;
    end
    n = n or 0;
    n = math.floor(n)
    local fmt = "%." .. n .. 'f'
    local nRet = tonumber(string.format(fmt, nNum))
    return nRet;
end

function GetPetGrowUp(petindex)--计算成长率函数
	local GrowUp = 0;
	local level = char.getInt(petindex, "等级") - char.getInt(petindex, "宠初等级");
	if level > 0 then
		local InitAtt1 = char.getInt(petindex, "宠初耐久");
		local InitAtt2 = char.getInt(petindex, "宠初攻击");
		local InitAtt3 = char.getInt(petindex, "宠初防御");
		local InitAtt4 = char.getInt(petindex, "宠初敏捷");
		local atk= GetPreciseDecimal((char.getWorkInt(petindex, "攻击力") - InitAtt2)/level,4);
		local def= GetPreciseDecimal((char.getWorkInt(petindex, "防御力") - InitAtt3)/level,4);
		local quick= GetPreciseDecimal((char.getWorkInt(petindex, "敏捷力") -InitAtt4)/level,4);
		GrowUp = atk+def+quick;
	end
	return GrowUp;
end


function SetSendToken(_Typeid,_Petid,_Playid,_Attid,_TypePage,_PetPage)--设置发送数据文本
	--获取榜单信息
	--参数0为窗口选项记录信息(1类型选择，2宠物选择，3排名选择，4排序选择，5类型页面，6宠物页面，7类型最大,8宠物最大)
	local mytypeid = _Typeid;--获取类型选择
	local mypetid = _Petid;--获取宠物选择
	local myplayid = _Playid;--获取排名选择
	local myattid = _Attid;--获取排序选择
	local mytypepage = _TypePage;--获取当前类型页面
	local mypetpage = _PetPage;--获取当前宠物页面
			
	local typemaxpage = math.ceil(table.getn(PetList) / 5);--计算宠物类型最大页数
	local tmtypeid = ((mytypepage-1)*5);--当前选择的类型页数索引
	local tmpetid = ((mypetpage-1)*5);--当前选择的宠物页数索引
	local petmaxpage = math.ceil(table.getn(PetList[tmtypeid+mytypeid][2]) / 5);--计算宠物数据最大页数

	local token = mytypeid.."|"..mypetid.."|"..myplayid.."|"..myattid.."|"..mytypepage.."|"..mypetpage.."|"..typemaxpage.."|"..petmaxpage.."\n";
	--参数1-5为宠物类型项
	for i=tmtypeid+1,tmtypeid+5 do
		if PetList[i] == nil then
			token = token .. "无\n";
		else
			token = token .. PetList[i][1].."\n";
		end
	end
			--参数6-10为宠物类型项
	for i=tmpetid+1,tmpetid+5 do
		if PetList[tmtypeid+mytypeid][2][i] == nil then
			token = token .. "无\n";
		else
			token = token .. pet.getDataChar(PetList[tmtypeid+mytypeid][2][i], "名字").."\n";
		end
	end
	--参数11-15为排名玩家
	local getpetid = PetList[tmtypeid+mytypeid][2][tmpetid+mypetid];--取得要从数据库获取的宠物enemybase编号
	local sqlmsg = "";
	if myattid == 1 then--按战斗力排序
		sqlmsg = "SELECT * FROM `Pet_RanKing` WHERE `EnemyBaseId`="..getpetid.." order by Score Desc LIMIT 0,5";
	elseif myattid == 2 then--按耐久力排序
		sqlmsg = "SELECT * FROM `Pet_RanKing` WHERE `EnemyBaseId`="..getpetid.." order by Att1,Score Desc LIMIT 0,5";
	elseif myattid == 3 then--按攻击力排序
		sqlmsg = "SELECT * FROM `Pet_RanKing` WHERE `EnemyBaseId`="..getpetid.." order by Att2,Score Desc LIMIT 0,5";
	elseif myattid == 4 then--按防御力排序
		sqlmsg = "SELECT * FROM `Pet_RanKing` WHERE `EnemyBaseId`="..getpetid.." order by Att3,Score Desc LIMIT 0,5";
	elseif myattid == 5 then--按敏捷力排序
		sqlmsg = "SELECT * FROM `Pet_RanKing` WHERE `EnemyBaseId`="..getpetid.." order by Att4,Score Desc LIMIT 0,5";
	end
	local petname = pet.getDataChar(getpetid, "名字");
	local petmoto = pet.getDataInt(getpetid, "形象");
	local petdsx = pet.getDataInt(getpetid, "地");
	local petssx = pet.getDataInt(getpetid, "水");
	local pethsx = pet.getDataInt(getpetid, "火");
	local petfsx = pet.getDataInt(getpetid, "风");
	
	local playername = {"无","无","无","无","无"};
	local tmpetdata = {petname,"",petmoto,petdsx,petssx,pethsx,petfsx,0,0,0,0,0,0,0,0,0,0,0,"",0};
	
	local tmret = sasql.query(sqlmsg);--执行查询
	if tmret == 1 then
		sasql.free_result();
		sasql.store_result();
		local tmnum = sasql.num_rows();
		for i=1,5 do
			if i <= tmnum then
				sasql.fetch_row();
				playername[i] = sasql.data(2);
				if i == myplayid then
					--1宠物名字|2玩家名
					--3宠物图像|4地属性|5水属性|6火属性|7风属性|8转数
					--9宠物初等|10宠物初血|11宠物初攻|12宠物初防|13宠物初敏
					--14宠物现等|15宠物现血|16宠物现攻|17宠物现防|18宠物现敏
					--19宠物成长|20宠物评分
					tmpetdata[1] = sasql.data(1);
					tmpetdata[2] = sasql.data(2);
					tmpetdata[3] = other.atoi(sasql.data(5));--图像
					tmpetdata[4] = other.atoi(sasql.data(16));--地属性
					tmpetdata[5] = other.atoi(sasql.data(17));--水属性
					tmpetdata[6] = other.atoi(sasql.data(18));--火属性
					tmpetdata[7] = other.atoi(sasql.data(19));--风属性
					tmpetdata[8] = other.atoi(sasql.data(22));--转生
					tmpetdata[9] = other.atoi(sasql.data(6));--初始等级
					tmpetdata[10] = other.atoi(sasql.data(7));--初始血
					tmpetdata[11] = other.atoi(sasql.data(8));--初始攻
					tmpetdata[12] = other.atoi(sasql.data(9));--初始防
					tmpetdata[13] = other.atoi(sasql.data(10));--初始敏
					tmpetdata[14] = other.atoi(sasql.data(11));--当前等级
					tmpetdata[15] = other.atoi(sasql.data(12));--当前血
					tmpetdata[16] = other.atoi(sasql.data(13));--当前攻
					tmpetdata[17] = other.atoi(sasql.data(14));--当前防
					tmpetdata[18] = other.atoi(sasql.data(15));--当前敏
					tmpetdata[19] = sasql.data(20);--成长率
					tmpetdata[20] = other.atoi(sasql.data(21));--战斗力
					
				end
			end
		end
	end
			
	for i=1,5 do
		token = token .. playername[i] .."\n";
	end
	--15以上为宠物数据用
	--1宠物名字|2玩家名
	--1宠物图像|2地属性|3水属性|4火属性|5风属性|6转数
	--1宠物初等|2宠物初血|3宠物初攻|4宠物初防|5宠物初敏
	--1宠物现等|2宠物现血|3宠物现攻|4宠物现防|5宠物现敏
	--1宠物成长|2宠物评分
	token = token .. tmpetdata[1].."|"..tmpetdata[2].."\n";
	token = token .. tmpetdata[3].."|"..tmpetdata[4].."|"..tmpetdata[5].."|"..tmpetdata[6].."|"..tmpetdata[7].."|"..tmpetdata[8].."\n";
	token = token .. tmpetdata[9].."|"..tmpetdata[10].."|"..tmpetdata[11].."|"..tmpetdata[12].."|"..tmpetdata[13].."\n";
	token = token .. tmpetdata[14].."|"..tmpetdata[15].."|"..tmpetdata[16].."|"..tmpetdata[17].."|"..tmpetdata[18].."\n";
	token = token .. tmpetdata[19].."|"..tmpetdata[20].."\n";
	--print(token);
	return token;
end

------------------------------- 主NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	--print( _meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) ~= 1 then
		return ;
	end
	if (_select == 8) then
		return ;
	end
	if (_seqno == 0) then
		local setid = other.atoi(_data);
		if setid == 1 then--查看榜单
			local token = SetSendToken(1,1,1,1,1,1);
			--print(token);
			lssproto.windows(_talkerindex, 1015, 0,2, char.getWorkInt( _meindex, "对象"), token);
		elseif setid == 2 then--提交宠物
			lssproto.windows(_talkerindex, "宠物框", 8, 1, char.getWorkInt( _meindex, "对象"), "");
		end
	elseif (_seqno == 1) then--提交宠物
		local petid = other.atoi(_data)-1;
		local petindex = char.getCharPet(_talkerindex, petid);--获取宠物索引
		if char.check(petindex) == 1 then--判断该索引宠物是否存在
			local InitLv = char.getInt(petindex, "宠初等级");
			local InitAtt1 = char.getInt(petindex, "宠初耐久");
			local InitAtt2 = char.getInt(petindex, "宠初攻击");
			local InitAtt3 = char.getInt(petindex, "宠初防御");
			local InitAtt4 = char.getInt(petindex, "宠初敏捷");
			local GrowUp = GetPetGrowUp(petindex);
			local Score = (char.getWorkInt(petindex, "最大HP")/4)+char.getWorkInt(petindex, "攻击力")+char.getWorkInt(petindex, "防御力")+char.getWorkInt(petindex, "敏捷力");
			
			--先查询该宠物是否有记录
			local token = "select Att1,PlayerName from `Pet_RanKing` where `Unicode` = '"..char.getChar(petindex, "唯一编号").."' limit 1";
			local ret = sasql.query(token);
			if ret == 1 then
				sasql.free_result();
				sasql.store_result();
				local num = sasql.num_rows();
				if num == 0 then--这个宠物数据没有存在，直接提交他
					local token = "INSERT INTO `Pet_RanKing` ("
								.."`PetName`"
								..",`PlayerName`"
								..",`PlayerAccount`"
								..",`EnemyBaseId`"
								..",`Image`"
								..",`InitLv`"
								..",`InitAtt1`"
								..",`InitAtt2`"
								..",`InitAtt3`"
								..",`InitAtt4`"
								..",`Lv`"
								..",`Att1`"
								..",`Att2`"
								..",`Att3`"
								..",`Att4`"
								..",`Land`"
								..",`Water`"
								..",`Fire`"
								..",`Wind`"
								..",`GrowUp`"
								..",`Score`"
								..",`Trans`"
								..",`Unicode`"
								..") VALUES ("
								.."'" .. char.getChar(petindex, "名字") .. "'"
								..",'".. char.getChar(_talkerindex, "名字") .. "'"
								..",'".. char.getChar(_talkerindex, "账号") .. "'"
								..",".. char.getInt(petindex, "宠ID")
								..",".. char.getInt(petindex, "图像号")
								..",".. InitLv
								..",".. InitAtt1
								..",".. InitAtt2
								..",".. InitAtt3
								..",".. InitAtt4
								..",".. char.getInt(petindex, "等级")
								..",".. char.getWorkInt(petindex, "最大HP")
								..",".. char.getWorkInt(petindex, "攻击力")
								..",".. char.getWorkInt(petindex, "防御力")
								..",".. char.getWorkInt(petindex, "敏捷力")
								..",".. char.getInt(petindex, "地")
								..",".. char.getInt(petindex, "水")
								..",".. char.getInt(petindex, "火")
								..",".. char.getInt(petindex, "风")
								..",".. GrowUp
								..",".. math.floor(Score)
								..",".. char.getInt(petindex, "转数")
								..",'".. char.getChar(petindex, "唯一编号") .. "'"
								.. ")";
					--print(token);
					sasql.query(token);
					lssproto.windows(_talkerindex, "对话框", "取消",-1, char.getWorkInt( _meindex, "对象"), "\n\n宠物提交成功！\n如果未在排行榜中显示，请将下面编号提交给管理员！\n[style c=6]"..char.getInt(petindex, "宠ID").."[/style]");
					return ;
				else--这个宠物数据已经存在，判断属性是否需要更新提交
					sasql.fetch_row();
					local hp = other.atoi(sasql.data(1));
					local PlayerName = sasql.data(2);
					if hp == char.getWorkInt(petindex, "最大HP") and char.getChar(_talkerindex, "名字") == PlayerName then
						lssproto.windows(_talkerindex, "对话框", "取消",-1, char.getWorkInt( _meindex, "对象"), "\n\n您的宠物尚未发生变化！\n如果未在排行榜中显示，请将下面编号提交给管理员！\n[style c=6]"..char.getInt(petindex, "宠ID").."[/style]");
						return ;
					else
						local token = "UPDATE `Pet_RanKing` "
									.. "SET `PetName` = '" .. char.getChar(petindex, "名字").."'"
									.. ",`PlayerName` = '" .. char.getChar(_talkerindex, "名字").."'"
									.. ",`PlayerAccount` = '" .. char.getChar(_talkerindex, "账号").."'"
									.. ",`EnemyBaseId` = " .. char.getInt(petindex, "宠ID")
									.. ", `Image` = " .. char.getInt(petindex, "图像号")
									.. ", `InitLv` = " .. InitLv
									.. ", `InitAtt1` = " .. InitAtt1
									.. ", `InitAtt2` = " .. InitAtt2
									.. ", `InitAtt3` = " .. InitAtt3
									.. ", `InitAtt4` = " .. InitAtt4
									.. ", `Lv` = " .. char.getInt(petindex, "等级")
									.. ", `Att1` = " .. char.getWorkInt(petindex, "最大HP")
									.. ", `Att2` = " .. char.getWorkInt(petindex, "攻击力")
									.. ", `Att3` = " .. char.getWorkInt(petindex, "防御力")
									.. ", `Att4` = " .. char.getWorkInt(petindex, "敏捷力")
									.. ", `Land` = " .. char.getInt(petindex, "地")
									.. ", `Water` = " .. char.getInt(petindex, "水")
									.. ", `Fire` = " .. char.getInt(petindex, "火")
									.. ", `Wind` = " .. char.getInt(petindex, "风")
									.. ", `GrowUp` = " .. GrowUp
									.. ", `Score` = " .. math.floor(Score)
									.. ", `Trans` = " .. char.getInt(petindex, "转数")
									.. " WHERE `Unicode` = '" .. char.getChar(petindex, "唯一编号") .. "'";
						--print(token);
						sasql.query(token);
						lssproto.windows(_talkerindex, "对话框", "取消",-1, char.getWorkInt( _meindex, "对象"), "\n\n更新宠物记录成功！\n如果未在排行榜中显示，请将下面编号提交给管理员！\n[style c=6]"..char.getInt(petindex, "宠ID").."[/style]");
						return ;
					end
				end
			end
		end
	elseif (_seqno == 2) then--窗口二次操作
		local tmdata = StringSplit(_data,"|");--分割数据
		if tmdata[1] == "类型选择" then
			if table.getn(tmdata) > 3 then
				local lstypeid = other.atoi(tmdata[2]);
				local lstypepage = other.atoi(tmdata[3]);
				local lsattid = other.atoi(tmdata[4]);
				--SetSendToken(_Typeid,_Petid,_Playid,_Attid,_TypePage,_PetPage)--设置发送数据文本
				local token = SetSendToken(lstypeid,1,1,lsattid,lstypepage,1);
				--print(token);
				lssproto.windows(_talkerindex, 1015, 0,2, char.getWorkInt( _meindex, "对象"), token);
			end
			return ;
		elseif tmdata[1] == "类型翻页" then
			if table.getn(tmdata) > 2 then
				local lstypepage = other.atoi(tmdata[2]);
				local lsattid = other.atoi(tmdata[3]);
				--SetSendToken(_Typeid,_Petid,_Playid,_Attid,_TypePage,_PetPage)--设置发送数据文本
				local token = SetSendToken(1,1,1,lsattid,lstypepage,1);
				--print(token);
				lssproto.windows(_talkerindex, 1015, 0,2, char.getWorkInt( _meindex, "对象"), token);
			end
			return ;
		elseif tmdata[1] == "宠物选择" then
			if table.getn(tmdata) > 5 then
				--类型选择传递4个参数 当前宠物选择项，类型页码，类型选择，宠物选择页码，选择排序
				local lspetid = other.atoi(tmdata[2]);--类型页码
				local lstypepage = other.atoi(tmdata[3]);--类型页码
				local lstypeid = other.atoi(tmdata[4]);--类型选择
				local lspetpage = other.atoi(tmdata[5]);--宠物选择
				local lsattid = other.atoi(tmdata[6]);--排序选择
				--SetSendToken(_Typeid,_Petid,_Playid,_Attid,_TypePage,_PetPage)--设置发送数据文本
				local token = SetSendToken(lstypeid,lspetid,1,lsattid,lstypepage,lspetpage);
				--print(token);
				lssproto.windows(_talkerindex, 1015, 0,2, char.getWorkInt( _meindex, "对象"), token);
			end
		elseif tmdata[1] == "宠物翻页" then
			if table.getn(tmdata) > 4 then
				local lspetpage = other.atoi(tmdata[2]);--类型页码
				local lstypepage = other.atoi(tmdata[3]);--类型页码
				local lstypeid = other.atoi(tmdata[4]);--类型选择
				local lsattid = other.atoi(tmdata[5]);--排序选择
				--SetSendToken(_Typeid,_Petid,_Playid,_Attid,_TypePage,_PetPage)--设置发送数据文本
				local token = SetSendToken(lstypeid,1,1,lsattid,lstypepage,lspetpage);
				--print(token);
				lssproto.windows(_talkerindex, 1015, 0,2, char.getWorkInt( _meindex, "对象"), token);
				
			end
		elseif tmdata[1] == "排序选择" then
			if table.getn(tmdata) > 5 then
				local lsattid = other.atoi(tmdata[2]);--排序选项
				local lstypeid = other.atoi(tmdata[3]);--类型选项
				local lstypepage = other.atoi(tmdata[4]);--类型页面
				local lspetid = other.atoi(tmdata[5]);--宠物选择
				local lspetpage = other.atoi(tmdata[6]);--宠物页面
				--SetSendToken(_Typeid,_Petid,_Playid,_Attid,_TypePage,_PetPage)--设置发送数据文本
				local token = SetSendToken(lstypeid,lspetid,1,lsattid,lstypepage,lspetpage);
				--print(token);
				lssproto.windows(_talkerindex, 1015, 0,2, char.getWorkInt( _meindex, "对象"), token);
			end
		elseif tmdata[1] == "排名选择" then
			if table.getn(tmdata) > 6 then
				--类型选择传递4个参数 当前宠物选择项，类型页码，类型选择，宠物选择页码，选择排序
				local lspetid = other.atoi(tmdata[2]);--类型页码
				local lstypepage = other.atoi(tmdata[3]);--类型页码
				local lstypeid = other.atoi(tmdata[4]);--类型选择
				local lspetpage = other.atoi(tmdata[5]);--宠物选择
				local lsattid = other.atoi(tmdata[6]);--排序选择
				local lsplayid = other.atoi(tmdata[7]);--宠物页面
				--SetSendToken(_Typeid,_Petid,_Playid,_Attid,_TypePage,_PetPage)--设置发送数据文本
				local token = SetSendToken(lstypeid,lspetid,lsplayid,lsattid,lstypepage,lspetpage);
				--print(token);
				lssproto.windows(_talkerindex, 1015, 0,2, char.getWorkInt( _meindex, "对象"), token);
			end
		end
	end
end

------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		local token = "3\n                  [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n"
					.."\n"
					.."\n                【 查看宠物排行 】"
					.."\n                【 提交宠物数据 】";
		lssproto.windows(_talkerindex, 2, 8, 0, char.getWorkInt( _meindex, "对象"), token);
	end
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
	--宠物类型列表
	--类型名,宠物enemybase列表
	PetList = {{"乌力系",{1,2,3,4,5,6,725,726,727}}
			  ,{"布伊系",{11,12,13,14,15,728,729,730}}
			  ,{"加美系",{21,22,23,24,731,732,733,734}}
			  ,{"威威系",{31,32,33,34,735,736,737,738}}
			  ,{"舌头系",{41,42,43,44,739,740,741,742}}
			  ,{"石龟系",{51,52,53,54,743,744,745,746}}
			  ,{"拳头系",{61,62,63,64,65,747,748,749}}
			  ,{"跳狗系",{71,72,73,74,750,751,752,753}}
			  ,{"穿山甲系",{81,82,83,84,85,86}}
			  ,{"人龙系",{91,92,93,94,95,754,755,756}}
			  ,{"兔子系",{101,102,103,104,757,758,759,760}}
			  ,{"凯比系",{111,112,113,114,761,762,763,764}}
			  ,{"猩猩系",{141,142,143,144,765,766,767,768}}
			  ,{"布克系",{171,172,173,174,769,770,771}}
			  ,{"茄子系",{181,182,183,184,772,773,774,775}}
			  ,{"老虎系",{191,192,193,194,776,777,778,779}}
			  ,{"犀牛系",{211,212,213,214,780,781,782,783}}
			  ,{"鲨鱼系",{221,222,223,224,784,785,786,787}}
			  ,{"毒蛙系",{231,232,233,234,788,789,790,791}}
			  ,{"海主人系",{241,242,243,244,792,793,794,795}}
			  ,{"雷龙系",{251,252,253,254,796,797,798}}
			  ,{"大象系",{261,262,263,264,799,800,801,802}}
			  ,{"飞龙系",{271,272,273,274,803,804,805,806}}
			  ,{"奇宝系",{291,292,293,294,807,808,809,810}}
			  ,{"暴龙系",{301,302,303,304,811,812,813,814}}
			  ,{"羚羊系",{311,312,313,314,815,816,817}}
			  ,{"猪头鱼系",{321,322,323,324,325,818,819,820}}
			  ,{"水龙系",{331,332,333,334,821,822,823,824}}
			  ,{"马年系",{901,902,903,904}}
			  ,{"羊年系",{1056,1057,1058,1059}}
			  ,{"鸡年系",{1212,1213,1214,1215}}
			  ,{"狼人系",{981,982,983,984}}
			  ,{"魔兽系",{985,986,1047,1048,1060}}
			  ,{"米老鼠系",{1049,1050}}
			  ,{"火牛系",{1051}}
			  ,{"地蜥蜴系",{1052,1053,1054,1055}}
			  ,{"人鱼系",{1116,1117,1118,1119}}
			  ,{"蜜蜂系",{1178,1179}}
			  ,{"蝴蝶系",{1180,1181}}
			  ,{"蚁人系",{1184,1185}}
			  ,{"牛人系",{1186,1187}}
			  ,{"甲虫系",{1294,1295}}
			  ,{"海豚系",{1296,1297}}
			  ,{"鳄鱼系",{1298,1299}}
			  ,{"罗莉喵系",{1188,1189}}
			  ,{"海底系1",{1065,1066,1067,1068,1069,1070,1071,1072,1073,1074,1075,1076}}
			  ,{"海底系2",{1079,1080,1081,1082,1083,1084,1085,1086,1087,1088,1089,1090,1091,1092}}
			  ,{"海底系3",{1095,1096,1097,1098,1099,1100}}
			  ,{"海底系4",{1103,1104,1105,1106}}
			  ,{"海底系5",{1109,1110,1111,1112,1113,1114}}
			  ,{"陆融系1",{989,990,991,992,993,994,995,996,997,998,999,1000,1001,1002,1003,1004,1005,1006,1007,1008,1009,1010,1011,1012}}
			  ,{"陆融系2",{1013,1014,1015,1016,1017,1018,1019,1020,1021,1022,1023,1024,1025,1026,1027,1028,1029,1030,1031,1032,1033}}
			  };
	npcdata = {"宠物排行榜",51138,777,47,41,6,"PetRanking"};
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end
