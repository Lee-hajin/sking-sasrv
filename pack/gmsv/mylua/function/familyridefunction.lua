function FamilyRideCheck( meindex,petindex,pethaveindex)
	local petNo = char.getInt( petindex,"原图像号");
	local no = CheckPetRideByPetNo( meindex,petNo);
	if no > 0 then
		return 1;
	end
	return 0;
end


function FreeRideQuery(charaindex)
	FreeRideQuery2(charaindex,1);
end

function FreeRideQuery2(charaindex,_page)
	local petNo = 0;
	local token = "";
	local numtop = 1+((_page-1)*16);--起始数据位置
	local nummax = 16+((_page-1)*16);--结束数据位置
	for i=numtop,nummax do
		if ridedata[i] ~= nil then
			token = token .. i.."|"..ridedata[i][1].."|"..ridedata[i][2].."|"..CheckPetRideByPetNo( charaindex,ridedata[i][2]).."|"..CheckRideNoByPetNo( charaindex,ridedata[i][2]).."\n";
		end
	end
	lssproto.windows(charaindex,1014,1,_page,char.getWorkInt( npcindex,"对象"),token);
	--return 1;
end


function WindowTalked( meindex,talkerindex,seqno,select,data)
	if seqno >= 1 then
		if select == 48 then
			local pagenum = seqno+2;
			if pagenum > math.ceil(table.getn(ridedata)/16) then
				pagenum = 1;
			end
			FreeRideQuery2(talkerindex,pagenum);
		elseif select == 32 then
			local pagenum = seqno+1;
			if pagenum > math.ceil(table.getn(ridedata)/16) then
				pagenum = 1;
			end
			FreeRideQuery2(talkerindex,pagenum);
		elseif select == 16 then
			local pagenum = seqno-1;
			if pagenum < 1 then
				pagenum = math.ceil(table.getn(ridedata)/16);
			end
			FreeRideQuery2(talkerindex,pagenum);
		end		
		print(pagenum);
		print(seqno);
	end
end


function CheckRideNoByPetNo( meindex,petNo)
	local playerid = getNOindex(char.getInt( meindex,"原图像号"));
	if playerid > 0 then
		for i = 1,table.getn(ridedata) do
			if petNo == ridedata[i][2] then
				--print(NewRideNoList[playerid][i]);
				return ridedata[i][3][playerid];
			end
		end

		for i = 1,table.getn(PlayerColor[1]) do
			if PlayerColor[playerid][i] == char.getInt( meindex,"原图像号") or (char.getInt( meindex,"原图像号") >= 100700 and char.getInt( meindex,"原图像号") < 108400) then
				if petNo == CommonPetList[i] then
					--print(CommonRideNoList[playerid][i]);
					return CommonRideNoList[playerid][i];
				end
			end
		end

		for i = 1,table.getn(CommonPetList1) do
			if char.getInt( meindex,"原图像号") == CommonPetList1[i][2] and petNo == CommonPetList1[i][3] then
				--print(CommonPetList1[i][1]);
				return CommonPetList1[i][1];
			end
		end


		for i = 1,table.getn(Pointpetlist) do
			if petNo == Pointpetlist[i][1] then
				floorid = char.getWorkInt( meindex,"家族地图")
				for j = 1,table.getn(Pointpetlist[i][2]) do
					if Pointpetlist[i][2][j] == floorid then
						if char.getInt( meindex,"家族地位") > 0 and char.getInt( meindex,"家族地位") ~= 2 then
							if char.getInt( meindex,"家族类型") == Pointpetlist[i][3] then
								if PointRideNoList[playerid][i] > -1 then
									--print(PointRideNoList[playerid][i]);
									return PointRideNoList[playerid][i];
								end
							end
						end
					end
				end
			end
		end
	end
	return -1;
end

function CheckPetRideByPetNo( meindex,petNo)
	local playerid = getNOindex(char.getInt( meindex,"原图像号"))

	if char.getInt( meindex,"图像号") == 101178 then
		if petNo == 101177 then
			return 101179;
		end
	elseif char.getInt( meindex,"图像号") == 101177 then
		if petNo == 101178 then
			return 101179;
		end
	elseif playerid > 0 then
		for i = 1,table.getn(ridedata) do
			if petNo == ridedata[i][2] then
				if other.DataAndData(char.getInt(meindex,"证书骑宠"),i - 1) ~= 0 then
					return 1;
				end
				if other.DataAndData(char.getInt(meindex,"证书骑宠1"),i - 1) ~= 0 then
					return 1;
				end
				if other.DataAndData(char.getInt(meindex,"证书骑宠2"),i - 1) ~= 0 then
					return 1;
				end
			end
		end

		for i = 1,table.getn(PlayerColor[1]) do
			if PlayerColor[playerid][i] == char.getInt( meindex,"原图像号") or (char.getInt( meindex,"原图像号") >= 100700 and char.getInt( meindex,"原图像号") < 108400) then
				if petNo == CommonPetList[i] then
					return 1
				end
			end
		end

		for i = 1,table.getn(CommonPetList1) do
			if char.getInt( meindex,"原图像号") == CommonPetList1[i][2] and petNo == CommonPetList1[i][3] then
				return 1;
			end
		end


		for i = 1,table.getn(Pointpetlist) do
			if petNo == Pointpetlist[i][1] then
				floorid = char.getWorkInt( meindex,"家族地图")
				for j = 1,table.getn(Pointpetlist[i][2]) do
					if Pointpetlist[i][2][j] == floorid then
						if char.getInt( meindex,"家族地位") > 0 and char.getInt( meindex,"家族地位") ~= 2 then
							if char.getInt( meindex,"家族类型") == Pointpetlist[i][3] then
								if PointRideNoList[playerid][i] > -1 then
									return 1
								end
							end
						end
					end
				end
			end
		end
	end
	return 0

end

--此LUA为骑宠判断回调函数，返回0为不可骑乘，返回其他为可骑乘
function FamilyRideFunction( meindex,petindex,pethaveindex)
	--if char.getInt( petindex,"转数") >1 then
		--char.TalkToCli(meindex,-1,"【系统提示】由于[" .. char.getChar(petindex,"名字") .. "]已经转生，所以无法骑乘。","红色")
		--return 0
	--end
	--if char.getInt(petindex,"提升值") ~= 0 then
		--char.TalkToCli(meindex,-1,"【系统提示】由于[" .. char.getChar(petindex,"名字") .. "]已经祝福，所以无法骑乘。","红色")
		--return 0
	--end
	for i=0,6 do
		if char.getItemIndex(petindex,0) ~= -1 then
			char.TalkToCli(meindex,-1,"【系统提示】该宠物身上有装备，所以无法骑乘。","红色");
			return 0;
		end
	end
	--print(char.getItemIndex(petindex,0));
	local playermoto = char.getInt( meindex,"原图像号");
	if playermoto < 0 and playermoto > 110000 then
		return 0;
	end
	local playerid = getNOindex(playermoto)
	
	local petNo = char.getInt( petindex,"原图像号")
	if char.getInt( meindex,"图像号") == 101178 then
		if petNo == 101177 then
			return 101179;
		end
	elseif char.getInt( meindex,"图像号") == 101177 then
		if petNo == 101178 then
			return 101179;
		end
	elseif playerid > 0 then
		for i = 1,table.getn(ridedata) do
			if petNo == ridedata[i][2] then
				if other.DataAndData(char.getInt(meindex,"证书骑宠"),i - 1) ~= 0 then
					return ridedata[i][3][playerid];
				end
				if other.DataAndData(char.getInt(meindex,"证书骑宠1"),i - 1) ~= 0 then
					return ridedata[i][3][playerid];
				end
				if other.DataAndData(char.getInt(meindex,"证书骑宠2"),i - 1) ~= 0 then
					return ridedata[i][3][playerid];
				end
			end
		end
		for i = 1,table.getn(PlayerColor[1]) do
			if PlayerColor[playerid][i] == playermoto or (playermoto >= 100700 and playermoto < 108400) then
				if petNo == CommonPetList[i] then
					return CommonRideNoList[playerid][i];
				end
			end
		end

		for i = 1,table.getn(CommonPetList1) do
			if playermoto == CommonPetList1[i][2] and petNo == CommonPetList1[i][3] then
				return CommonPetList1[i][1];
			end
		end

		floorid = char.getWorkInt( meindex,"家族地图");
		for i = 1,table.getn(Pointpetlist) do
			if petNo == Pointpetlist[i][1] then
				for j = 1,table.getn(Pointpetlist[i][2]) do
					if Pointpetlist[i][2][j] == floorid then
						if char.getInt( meindex,"家族地位") > 0 and char.getInt( meindex,"家族地位") ~= 2 then
							if char.getInt( meindex,"家族类型") == Pointpetlist[i][3] then
								if PointRideNoList[playerid][i] > -1 then
									return PointRideNoList[playerid][i];
								end
							end
						end
					end
				end
			end
		end
	end
	return 0;
end



function getNOindex( baseNo)
	if baseNo >= 100000 and baseNo < 100240 then
		metamo = baseNo - 100000
		for i = 1,12 do
			if metamo >=  (i-2) * 20 and metamo <  i * 20 then
				return i;
			end
		end
	elseif baseNo >= 100700 and baseNo < 100820 then
		metamo = baseNo - 100700
		for i = 1,12 do
			if metamo >=  (i-2) * 10 and metamo <  i * 10 then
				return i;
			end
		end
	elseif baseNo >= 120000 and baseNo < 120020 then
		return 13;
	elseif baseNo >= 120100 and baseNo < 120105 then
		return 13;
	elseif baseNo >= 120110 and baseNo < 120115 then
		return 13;
	elseif baseNo >= 120020 and baseNo < 120040 then
		return 14;
	elseif baseNo >= 120105 and baseNo < 120110 then
		return 14;
	elseif baseNo >= 120115 and baseNo < 120120 then
		return 14;
	elseif baseNo >= 120040 and baseNo < 120060 then
		return 15;
	elseif baseNo >= 120060 and baseNo < 120080 then
		return 16;
	end
	return -1
end

function mydata()
	--骑证数据(1:骑宠名字,2:骑宠图号,3:12角色骑宠图)
	ridedata = {{"榜肺胶"	,100374,{104026,104027,104028,104029,104030,104031,104032,104033,104034,104035,104036,104037,130026,130027,130060,130061}}--0
			   ,{"公捞"	,100348,{130014,130015,130016,130017,130018,130019,130020,130021,130022,130023,130024,130025,130026,130027,130060,130061}}--1
			   ,{"档扼厚胶"		,100362,{130000,130001,130002,130003,130004,130005,130006,130007,130008,130009,130010,130011,130012,130013,130056,130057}}--2
			   ,{"绊福捣"		,100307,{130028,130029,130030,130031,130032,130033,130034,130035,130036,130037,130038,130039,130040,130041,130063,130064}}--3
			   ,{"扼绊绊"		,100279,{130042,130043,130044,130045,130046,130047,130048,130049,130050,130051,130052,130053,130054,130055,130068,130069}}--4
			   ,{"卡达鲁卡斯"	,100283,{101310,101318,101326,101334,101342,101350,101358,101366,101374,101382,101390,101398,130054,130055,130068,130069}}--5
			   ,{"卡卡金宝"		,100346,{101311,101319,101327,101335,101343,101351,101359,101367,101375,101383,101391,101399,130054,130055,130068,130069}}--6
			   ,{"格尔格"		,100310,{101312,101320,101328,101336,101344,101352,101360,101368,101376,101384,101392,101400,130054,130055,130068,130069}}--7
			   ,{"左迪洛斯"		,100372,{101008,101018,101028,101038,101048,101058,101068,101078,101088,101098,101108,101118,130054,130055,130068,130069}}--8
			   ,{"巴朵兰恩"		,100373,{101009,101019,101029,101039,101049,101059,101069,101079,101089,101099,101109,101119,130054,130055,130068,130069}}--9
			   ,{"史卡鲁"		,101532,{101978,101986,101975,101976,101984,101985,101977,101979,101981,101980,101983,101982,130054,130055,130068,130069}}--10
			   ,{"罗多克雷"		,101576,{101989,101988,101965,101966,101973,101974,101967,101968,101970,101969,101972,101971,130054,130055,130068,130069}}--11
			   ,{"斑尼迪克"		,100907,{103266,103262,103263,103264,103272,103273,103265,103267,103269,103268,103271,103270,130054,130055,130068,130069}}--12
			   ,{"邦浦洛斯"		,100289,{104065,104066,104067,104068,104069,104070,104071,104072,104073,104074,104075,104076,130054,130055,130068,130069}}--13
			   ,{"邦奇诺"		,100290,{104077,104078,104079,104080,104081,104082,104083,104084,104085,104086,104087,104088,130054,130055,130068,130069}}--14
			   ,{"利则诺顿"		,100287,{104101,104102,104103,104104,104105,104106,104107,104108,104109,104110,104111,104112,130054,130055,130068,130069}}--15
			   ,{"布鲁顿"		,100394,{104089,104090,104091,104092,104093,104094,104095,104096,104097,104098,104099,104100,130054,130055,130068,130069}}--16
			   ,{"斑尼鲁斯"		,106530,{104832,104833,104834,104835,104836,104837,104838,104839,104840,104841,104842,104843,130054,130055,130068,130069}}--17
			   ,{"乌宝依"		,100830,{104858,104858,104858,104858,104858,104858,104859,104859,104859,104859,104859,104859,130054,130055,130068,130069}}--18
			   ,{"威威"			,100264,{104860,104860,104860,104860,104860,104860,104860,104860,104860,104860,104860,104860,130054,130055,130068,130069}}--19
			   ,{"洛斯奇"		,103274,{104712,104713,104714,104715,104716,104717,104718,104719,104720,104721,104722,104723,130054,130055,130068,130069}}--20
			   ,{"洛斯德"		,103275,{104700,104701,104702,104703,104704,104705,104706,104707,104708,104709,104710,104711,130054,130055,130068,130069}}--21
			   ,{"洛斯恩"		,103276,{104724,104725,104726,104727,104728,104729,104730,104731,104732,104733,104734,104735,130054,130055,130068,130069}}--22
			   ,{"洛斯奈"		,103277,{104736,104737,104738,104739,104740,104741,104742,104743,104744,104745,104746,104747,130054,130055,130068,130069}}--23
			   ,{"格尔顿"		,100307,{104315,104316,104317,104318,104319,104320,104321,104322,104323,104324,104325,104326,130054,130055,130068,130069}}--24
			   ,{"蓝翼海龙兽"	,106403,{104460,104461,104462,104463,104464,104465,104466,104467,104468,104469,104470,104471,130054,130055,130068,130069}}--25
			   ,{"摩耶阎"		,106330,{104278,104278,104278,104278,104278,104278,104277,104277,104277,104277,104277,104277,130054,130055,130068,130069}}--26
			   ,{"摩耶悟"		,106329,{104263,104263,104263,104263,104263,104263,104264,104264,104264,104264,104264,104264,130054,130055,130068,130069}}--27
			   ,{"帖拉鲁卡"		,106527,{104796,104797,104798,104799,104800,104801,104802,104803,104804,104805,104806,104807,130054,130055,130068,130069}}--28
			   ,{"帖拉多斯"		,106528,{104808,104809,104810,104811,104812,104813,104814,104815,104816,104817,104818,104819,130054,130055,130068,130069}}--29
			   ,{"帖拉尼索"		,106524,{104688,104689,104690,104691,104692,104693,104694,104695,104696,104697,104698,104699,130054,130055,130068,130069}}--30
			   ,{"帖拉拉普"		,106525,{104760,104761,104762,104763,104764,104765,104766,104767,104768,104769,104770,104771,130054,130055,130068,130069}}--31
				};
	--判断玩家颜色
	PlayerColor = {{ 100000,100005,100010,100015}
				  ,{ 100025,100030,100035,100020}
				  ,{ 100055,100050,100045,100040}
				  ,{ 100060,100065,100070,100075}
				  ,{ 100095,100085,100090,100080}
				  ,{ 100100,100115,100110,100105}
				  ,{ 100135,100120,100125,100130}
				  ,{ 100145,100140,100150,100155}
				  ,{ 100165,100170,100160,100175}
				  ,{ 100190,100195,100185,100180}
				  ,{ 100200,100210,100215,100205}
				  ,{ 100230,100225,100220,100235}
				  ,{ 120000,120005,120010,120015}
				  ,{ 120020,120025,120030,120035}
				  ,{ 120040,120045,120050,120055}
				  ,{ 120060,120065,120070,120075}
				  };

	--骑虎数据(红虎,  绿虎,  金虎,  黄虎)
	CommonPetList = {100329,100327,100330,100328};
	CommonRideNoList = {{101004,101005,101006,101007}
						,{101015,101016,101017,101014}
						,{101027,101026,101025,101024}
						,{101034,101035,101036,101037}
						,{101047,101045,101046,101044}
						,{101054,101057,101056,101055}
						,{101067,101064,101065,101066}
						,{101075,101074,101076,101077}
						,{101085,101086,101084,101087}
						,{101096,101097,101095,101094}
						,{101104,101106,101107,101105}
						,{101116,101115,101114,101117}
						,{101085,101086,101084,101087}
						,{101096,101097,101095,101094}
						,{101104,101106,101107,101105}
						,{101116,101115,101114,101117}
						};
	---骑虎数据结束
	--骑雷数据				
	CommonPetList1  = {{ 101000,100000,100352}
						,{ 101001,100005,100352}
						,{ 101002,100010,100352}
						,{ 101003,100015,100352}
						,{ 101000,100700,100352}
						,{ 101000,100705,100352}
						
						,{ 101010,100020,100396}
						,{ 101011,100025,100396}
						,{ 101012,100030,100396}
						,{ 101013,100035,100396}
						,{ 101010,100710,100396}
						,{ 101010,100715,100396}
						
						,{ 101020,100040,100351}
						,{ 101021,100045,100351}
						,{ 101022,100050,100351}
						,{ 101023,100055,100351}
						,{ 101020,100720,100351}
						,{ 101020,100725,100351}
						
						,{ 101030,100060,100353}
						,{ 101031,100065,100353}
						,{ 101032,100070,100353}
						,{ 101033,100075,100353}
						,{ 101030,100730,100353}
						,{ 101030,100735,100353}
						
						,{ 101040,100080,100396}
						,{ 101041,100085,100396}
						,{ 101042,100090,100396}
						,{ 101043,100095,100396}
						,{ 101040,100740,100396}
						,{ 101040,100745,100396}
						
						,{ 101050,100100,100353}
						,{ 101051,100105,100353}
						,{ 101052,100110,100353}
						,{ 101053,100115,100353}
						,{ 101050,100750,100353}
						,{ 101050,100755,100353}
						
						,{ 101060,100120,100354}
						,{ 101061,100125,100354}
						,{ 101062,100130,100354}
						,{ 101063,100135,100354}
						,{ 101060,100760,100354}
						,{ 101060,100765,100354}
						
						,{ 101070,100140,100354}
						,{ 101071,100145,100354}
						,{ 101072,100150,100354}
						,{ 101073,100155,100354}
						,{ 101070,100770,100354}
						,{ 101070,100775,100354}
						
						,{ 101080,100160,100352}
						,{ 101081,100165,100352}
						,{ 101082,100170,100352}
						,{ 101083,100175,100352}
						,{ 101080,100780,100352}
						,{ 101080,100785,100352}
						
						,{ 101090,100180,100351}
						,{ 101091,100185,100351}
						,{ 101092,100190,100351}
						,{ 101093,100195,100351}
						,{ 101090,100790,100351}
						,{ 101090,100795,100351}
						
						,{ 101100,100200,100353}
						,{ 101101,100205,100353}
						,{ 101102,100210,100353}
						,{ 101103,100215,100353}
						,{ 101100,100800,100353}
						,{ 101100,100805,100353}
						
						,{ 101110,100220,100396}
						,{ 101111,100225,100396}
						,{ 101112,100230,100396}
						,{ 101113,100235,100396}
						,{ 101110,100810,100396}
						,{ 101110,100815,100396}
						};
	---骑雷数据结束
	
	--庄园特骑(光明骑蓝暴，黑暗骑红暴)
	Pointpetlist = {{100372,{1041,2031,3031,4031,5031,6031,7031,8031,9031,7531},0}--蓝暴 
				   ,{100373,{1041,2031,3031,4031,5031,6031,7031,8031,9031,7531},1}--红暴
				   };
	                   --蓝暴, --红暴，--黑鸡，--黑鸡，--白斑，--白斑，--苍鹰，--苍鹰
	PointRideNoList = {{101008,101009}	--小矮子
					  ,{101018,101019}	--赛亚人
					  ,{101028,101029}	--辫子男孩
					  ,{101038,101039}	--酷哥
					  ,{101048,101049}	--熊皮男
					  ,{101058,101059}	--大个
					  ,{101068,101069}	--小矮妹
					  ,{101078,101079}	--熊皮妹
					  ,{101088,101089}	--帽子妹
					  ,{101098,101099}	--短法发夹妹
					  ,{101108,101109}	--手套女
					  ,{101118,101119}	--辣妹
					  ,{101088,101089}	--帽子妹
					  ,{101098,101099}	--短法发夹妹
					  ,{101108,101109}	--手套女
					  ,{101118,101119}	--辣妹
					  };
	--庄园特骑结束
	npcdata = {"骑宠查询模块",100000,777,32,22,6};	
end

function Create(_name,_metamo,_floor,_x,_y,_dir)
	npcindex = npc.CreateNpc(_name,_metamo,_floor,_x,_y,_dir)
	char.setFunctionPointer(npcindex,"窗口事件","WindowTalked","");
end

function reloadnpc()
	print("重读NPC信息",char.getChar(npcindex,"名字"));
	mydata();
	char.setChar(npcindex,"名字",npcdata[1]);
	char.setInt(npcindex,"图像号",npcdata[2]);
	char.setInt(npcindex,"原图像号",npcdata[2]);
	char.WarpToSpecificPoint(npcindex,npcdata[3],npcdata[4],npcdata[5]);
	char.setInt(npcindex,"方向",npcdata[6]);
	char.ToAroundChar(npcindex);
end

function main()
	mydata();
	Create(npcdata[1],npcdata[2],npcdata[3],npcdata[4],npcdata[5],npcdata[6]);
	magic.addLUAListFunction("重读骑宠数据","reloadnpc","",1,"[重读骑宠数据]");
end
