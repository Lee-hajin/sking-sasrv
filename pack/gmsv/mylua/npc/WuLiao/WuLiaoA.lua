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

------------------------------- 主NPC窗口事件函数 -------------------------------
function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) ~= 1 then
		return ;
	end
	if (_seqno == 0) then
		if _data == "1" then
			local token = "              [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n"
					.."\n1、超高配置三线服务器"
					.."\n2、最强大的辅助外挂"
					.."\n3、超高清还原石器所有画面"
					.."\n4、优良的软件服务一定会给您提供一个良好的游戏空间";
			lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), token);
		elseif _data == "2" then
			local token = "              [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n"
					.."\n1、本服经验倍数4倍 但是速度不会影响玩家体验"
					.."\n2、出生配备高级物品 水魔新手装备 便于练级"
					.."\n3、根据自己的等级选择练级地点"
					.."\n4、相信不需要多久你就可以达到6转140了";
			lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), token);
		elseif _data == "3" then
			local token = "2\n              [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n"
					.."\n1、商城简介"
					.."\n2、金币介绍"
					.."\n3、金币充值";
			lssproto.windows(_talkerindex, 2, 8, 1, char.getWorkInt( _meindex, "对象"), token);
		end
	elseif (_seqno == 1) then
		if _data == "1" then
			local token = "              [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n"
					.."\n    为了满足部分玩家需要以及维持服务器日常开销，每"
					.."\n个月托管维护费用，在不破坏游戏平衡的前提下，本服决"
					.."\n定开放商城";
			lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), token);
		elseif _data == "2" then
			local token = "              [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n"
					.."\n金币1点=人民币1元"
					.."\n金币可到金币商城进行消费"
					.."\n商城传送ESC键或医院进入";
			lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), token);
		elseif _data == "3" then
			local token = "              [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n"
					.."\n在我们的官方主页进行赞助充值"
					.."\n充值后可获得一个卡密"
					.."\n获得卡密后可在村长家（13.13）位置进行兑换";
			lssproto.windows(_talkerindex, 0, 8, -1, char.getWorkInt( _meindex, "对象"), token);
		end
	end
end

------------------------------- 主NPC对话事件函数 -------------------------------
function Talked(_meindex, _talkerindex , _szMes, _color )
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then 
		local token = "2\n              [style c=4]『" .. char.getChar(_meindex, "名字") .. "』[/style]"
					.."\n"
					.."\n1、石器Max 特色"
					.."\n2、石器新手入门"
					.."\n3、石器商城简介";
		lssproto.windows(_talkerindex, 2, 8, 0, char.getWorkInt( _meindex, "对象"), token);
	end
end

------------------------------- 主NPC创建事件函数 -------------------------------
function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
	char.setInt(npcindex,"现称号图像",npcdata[8]);
end

------------------------------- GM命令函数 -------------------------------
function reload(_charaindex,_data)
	local mytype = other.getString(_data, " ", 1);
	if mytype == "重读" then
		mydata();
		char.setChar(npcindex, "名字",npcdata[1]);
		char.setInt(npcindex, "图像号", npcdata[2]);
		char.setInt(npcindex, "原图像号", npcdata[2]);
		char.setInt(npcindex,"现称号图像",npcdata[8]);
		char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
		char.setInt(npcindex, "方向", npcdata[6]);
		char.ToAroundChar(npcindex);
		char.TalkToCli(_charaindex, -1, "重读NPC["..char.getChar(npcindex, "名字").."]完成", 6);
	end
end

------------------------------- 常量设置函数 -------------------------------
function mydata()
	npcdata = {"石器Max客服小姐",16322,777,42,21,4,"WuLiaoA",120001};
end

------------------------------- 启动函数 -------------------------------
function main()
	mydata();
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction(npcdata[7], "reload", "", 3, "["..npcdata[7].."]");
end
