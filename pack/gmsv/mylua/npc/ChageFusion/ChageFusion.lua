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

--_SetPage    选择页数
--_SetPet  选择项目
--_Pet1    选择主体
--_Pet2    选择客体1
--_Pet3    选择客体2
function SetSendToken(_talkerindex,SetPage,SetPetId,SetFusionId,_SetType)--设置发送数据文本
	local token = "";
	local MaxPage = 0;
	local FusionTable = {};
	if _SetType == 1 then
		FusionTable = FusionTable1;
	elseif _SetType == 2 then
		FusionTable = FusionTable2;
	end
	MaxPage = math.ceil(table.getn(FusionTable) / 5);--计算出总页数
	--1当前页数 2最大页数 3选择宠物 4选择造型
	token = SetPage.."|"..MaxPage.."|"..SetPetId.."|"..SetFusionId.."\n";
	--先发送宠物列表
	local petstr = {"无","无","无","无","无"};
	for i=1,5 do
		local petindex = char.getCharPet(_talkerindex, i-1);--主体索引
		if char.check(petindex) ~= 0 then
			petstr[i] = string.format( "%s-%d-%d-%d-%d-%d-%d"
						,char.getChar(petindex,"名字")
						,char.getInt(petindex, "等级")
						,char.getWorkInt(petindex, "最大HP")
						,char.getWorkInt(petindex, "攻击力")
						,char.getWorkInt(petindex, "防御力")
						,char.getWorkInt(petindex, "敏捷力")
						,char.getInt(petindex, "原图像号")
						);
		end
	end
	--整合宠物栏5只宠物信息
	for i=1,5 do
		token = token .. petstr[i] .."\n";
	end
	--发送5只融合选择
	local TopId = (SetPage-1)*5;--当前选择的类型页数索引
	for i=TopId+1,TopId+5 do
		if FusionTable[i] ~= nil then
			local enemybaseid = FusionTable[i][1];--取得enemybaseid
			token = token .. string.format( "%s-%d-%d-%d-%d-%d-%d\n"
							,pet.getDataChar(enemybaseid, "名字")
							,pet.getDataInt(enemybaseid, "形象")
							,pet.getDataInt(enemybaseid, "地")
							,pet.getDataInt(enemybaseid, "水")
							,pet.getDataInt(enemybaseid, "火")
							,pet.getDataInt(enemybaseid, "风")
							,FusionTable[i][3]
							);
		else
			token = token .. "无\n";
		end
	end
	--print(token);
	return token;
end
------------------------------- 主NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) ~= 1 then
		return ;
	end
	if _select == 8 then
		return ;
	end
	if _seqno == 0 then
		local setId = other.atoi(_data);
		
		local TmSetPage = 1;
		local TmSetPetId = 0;
		local TmSetFusionId = 0;
		local TmSetType = 0;
		if setId == 1 then
			TmSetType = 1;
		elseif setId == 3 then
			TmSetType = 2;
		end
		char.setWorkInt(_talkerindex,"临时整数1",TmSetType);
		local token = SetSendToken(_talkerindex,TmSetPage,TmSetPetId,TmSetFusionId,TmSetType);
		lssproto.windows(_talkerindex, 1019, 12, 1, char.getWorkInt( _meindex, "对象"), token);
	elseif _seqno == 1 then--陆地融合
		local tmdata = StringSplit(_data,"|");--分割数据
		if tmdata[1] == "选择翻页" then
			if table.getn(tmdata) < 4 then
				char.TalkToCli(_talkerindex, _meindex, "窗口信息传递有误，请重试[1]!", "红色");
				return;
			end
			local TmSetPage = other.atoi(tmdata[2]);
			local TmSetPetId = other.atoi(tmdata[3]);
			local TmSetFusionId = other.atoi(tmdata[4]);
			local TmSetType = char.getWorkInt(_talkerindex,"临时整数1");
			--SetSendToken(_talkerindex,SetPage,SetPetId,SetFusionId,_SetType)
			--选择翻页|"..SetPage.."|"..SetPetId.."|".. SetFusionId
			--token = SetPage.."|"..MaxPage.."|"..SetPetId.."|"..SetFusionId.."\n";
			--1融合类型 2选择页数 3选择项目 4选择主体 5选择客体1 6选择客体2 7最大页数
			local token = SetSendToken(_talkerindex,TmSetPage,TmSetPetId,TmSetFusionId,TmSetType);
			lssproto.windows(_talkerindex, 1019, 12, 1, char.getWorkInt( _meindex, "对象"), token);
		elseif tmdata[1] == "退出" then
			return ;
		elseif tmdata[1] == "确认更换" then
			if table.getn(tmdata) < 4 then
				char.TalkToCli(_talkerindex, _meindex, "窗口信息传递有误，请重试[1]!", "红色");
				return;
			end
			local TmSetPage = other.atoi(tmdata[2]);
			local TmSetPetId = other.atoi(tmdata[3]);
			local TmSetFusionId = other.atoi(tmdata[4]);
			local TmSetType = char.getWorkInt(_talkerindex,"临时整数1");
			local PetId = TmSetPetId - 1;--主体
			local MainIndex = char.getCharPet(_talkerindex, PetId);--取得宠物索引
			if char.getInt(MainIndex, "融合类型") <= 0 then
				char.TalkToCli(_talkerindex, _meindex, "该宠物非融合宠，无法进行外形变更!", "红色");
				return;
			end
			if char.getInt( MainIndex, "喂养次数") > 0 then
				char.TalkToCli(_talkerindex, _meindex, "请孵化之后再来进行外形变更!", "红色");
				return;
			end
			if char.getInt(MainIndex, "融合类型") ~= TmSetType then
				char.TalkToCli(_talkerindex, _meindex, "融合类型不一致，无法进行外形变更!", "红色");
				return;
			end
			local TMId = TmSetFusionId + ((TmSetPage-1)*5);--计算出选择索引
			local DelPoint = 0;
			local ChageMoto = -1;
			local ChageSX = {-1,-1,-1,-1};
			local ChageName = "";
			if TmSetType == 1 then
				if TMId > table.getn(FusionTable1) then
					char.TalkToCli(_talkerindex, _meindex, "选择超出范围！", "红色");
					return ;
				end
				DelPoint = FusionTable1[TMId][3];
				ChageMoto = pet.getDataInt(FusionTable1[TMId][1], "形象");
				ChageSX[1] = pet.getDataInt(FusionTable1[TMId][1], "地");
				ChageSX[2] = pet.getDataInt(FusionTable1[TMId][1], "水");
				ChageSX[3] = pet.getDataInt(FusionTable1[TMId][1], "火");
				ChageSX[4] = pet.getDataInt(FusionTable1[TMId][1], "风");
				ChageName = pet.getDataChar(FusionTable1[TMId][1], "名字");
			else
				if TMId > table.getn(FusionTable2) then
					char.TalkToCli(_talkerindex, _meindex, "选择超出范围！", "红色");
					return ;
				end
				DelPoint = FusionTable2[TMId][3];
				ChageMoto = pet.getDataInt(FusionTable2[TMId][1], "形象");
				ChageSX[1] = pet.getDataInt(FusionTable2[TMId][1], "地");
				ChageSX[2] = pet.getDataInt(FusionTable2[TMId][1], "水");
				ChageSX[3] = pet.getDataInt(FusionTable2[TMId][1], "火");
				ChageSX[4] = pet.getDataInt(FusionTable2[TMId][1], "风");
				ChageName = pet.getDataChar(FusionTable2[TMId][1], "名字");
			end
			if ChageMoto <= -1 then
				char.TalkToCli(_talkerindex, _meindex, "选择超出范围！", "红色");
				return ;
			end
			if ChageMoto == char.getInt(MainIndex, "原图像号") then
				char.TalkToCli(_talkerindex, _meindex, "你的宠物当前已经是这个造型了，无需更改！", "红色");
				return ;
			end
			local MyPoint = sasql.getVipPoint(_talkerindex);
			if MyPoint < DelPoint then
				char.TalkToCli(_talkerindex, _meindex, "你的金币不足扣除"..DelPoint.."点！", "红色");
				return ;
			end
			sasql.setVipPoint(_talkerindex,MyPoint-DelPoint);
			char.setInt(MainIndex, "图像号", ChageMoto);
			char.setInt(MainIndex, "原图像号", ChageMoto);
			char.setChar(MainIndex, "名字",ChageName);
			char.setInt(MainIndex, "地", ChageSX[1]);
			char.setInt(MainIndex, "水", ChageSX[2]);
			char.setInt(MainIndex, "火", ChageSX[3]);
			char.setInt(MainIndex, "风", ChageSX[4]);
			
			char.complianceParameter(MainIndex);
			char.sendStatusString(_talkerindex, "K" .. PetId);
			
			char.TalkToCli(_talkerindex, _meindex, "扣除 ".. DelPoint .." 金币，外形更换成功！", 4);
		end
	end
end

------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		local token = "3\n请选择要改变外形的融合类型：\n\n"
					.."\n               【陆地融合外形变更】"
					.."\n"
					.."\n               【海底融合外形变更】";
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
	--0为可参与任何融合，1为只可陆融，2为只可海融，3为只可空融
	--陆地融合表
	--1enemybaseid,2图像,3需求金币
	--禁止参与融合的技能
	FusionTable1 = {{989,101444,30},--库玛斯
					{990,101445,30},--雷玛特
					{991,101446,30},--菲斯玛
					{992,101447,30},--贝库尔
					--{993,101448,30},--史奈克特
					--{994,101449,30},--葛利斯奈
					--{995,101450,30},--艾柏瑞
					--{996,101451,30},--拉特利
					{997,101452,30},--毕格艾斯
					{998,101453,30},--裘比斯
					{999,101454,30},--阿卡西斯
					{1000,101455,30},--洛可洛斯
					{1001,101456,30},--法兹蒙特
					{1002,101457,30},--雷格特
					{1003,101458,30},--洛克尔
					{1004,101459,30},--米恩斯特
					{1005,101460,30},--利西亚
					{1006,101461,30},--佩罗亚
					{1007,101462,30},--伊利曼亚
					{1008,101463,30},--卡力洛亚
					{1009,101467,30},--奥普萨斯
					{1010,101465,30},--利普多尔
					{1011,101466,30},--邦克普斯
					{1012,101464,30},--拉佛洛斯
					{1013,101468,30},--迪亚古斯
					{1014,101469,30},--马鲁古斯
					{1015,101470,30},--尔洛普
					{1016,101471,30},--卡克普
					{1017,101472,30},--西修
					{1018,101473,30},--哈尔修
					{1019,101474,30},--弗利修
					{1020,101475,30},--巴利
					{1021,101476,30},--雅特拉斯
					{1022,101477,30},--伊利亚德
					{1023,101478,30},--米尼奈巴
					{1024,101479,30},--德拉斯
					{1025,101480,30},--科伦特
					{1026,101481,30},--邦欧里
					{1027,101482,30},--加雷
					{1028,101483,30},--加古特
					{1029,101484,30},--邦西普
					{1030,101485,30},--伊亚欧莱
					{1031,101486,30},--拉格多鲁
					{1032,101487,30},--西顿克尔
					{1033,101488,30},--克罗那斯
					};
	--海底融合表
	FusionTable2 = {{1190,101921,30},--艾克夫
					{1191,101918,30},--艾恩克
					{1192,101919,30},--艾美
					{1193,101920,30},--艾茵
					{1194,101914,30},--帕鲁吉
					{1195,101915,30},--尼玛吉
					{1196,101915,30},--威尔吉
					{1197,101914,30},--比斯吉
					{1198,101932,30},--布里斯卡
					{1199,101933,30},--邦斯卡
					{1200,101930,30},--奥斯卡
					{1201,101931,30},--斯斯卡
					{1202,101928,30},--塔塔拉
					{1203,101929,30},--吉姆拉
					{1204,101926,30},--伊弗拉
					{1205,101927,30},--欧拉欧拉
					{1206,101922,30},--安古力
					{1207,101923,30},--裘尼
					{1208,101924,30},--卡卡力
					{1209,101925,30},--亚瑟力
					};
	--不可遗传的技能
	NoPetSkill = {41,52,600,601,602,603,604,614,617,628,630,631,635,638,641};
	npcdata = {"融合造型师",51215,777,49,37,6,"NewFusion"};
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end
