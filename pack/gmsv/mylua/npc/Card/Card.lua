function StringMsg( _Data)--判断是否为数字和字母
	if( string.find(_Data,"'") or string.find(_Data,"\"") or string.find(_Data,"%c") or string.find(_Data,"%p") or string.find(_Data,"%s")) then
		return false;
	else
		return true;
	end
end

function Talked(meindex, talkerindex , szMes, color )
	if npc.isFaceToFace(meindex, talkerindex) == 1 then
		token = "卡密可通过我们官方网站赞助充值获得！"
			.."\n卡密一旦购买生成出来之后将无法退回！\n";
		if CardGiveJf[1] == true then
			token = token .."\n[style c=5]积分赠送活动中...[/style]\n"
					.."充值多少金币会额外赠送充值金币数的[style c=6]"..CardGiveJf[2].."%[/style]积分"
					.."\n如充值100金币，则会额外赠送100积分"
					.."\n[style c=4]请输入卡密信息进行充值:[/style]";
		else
			token = token .."\n\n\n\n\n[style c=4]请输入卡密信息进行充值:[/style]";
		end
		lssproto.windows(talkerindex, 1, 12, 1, char.getWorkInt( npcindex, "对象"), token)
	end
end

function WindowTalked( _meindex, _talkerindex, _seqno, _select, _data)
	if npc.isFaceToFace(_meindex, _talkerindex) == 1 then
		if _select == 8 then
			return;
		end
		if StringMsg(_data) == false then
			char.TalkToCli(_talkerindex, -1, "不可使用特殊字符！", "红色");
			return;
		end
		local datalen = string.len(_data);--取得卡密长度
		if( datalen ~= 40 ) then--卡密必须40位长度
			char.TalkToCli(_talkerindex, -1, "您输入的卡号无效！", "红色");
			return;
		end
		local token = "select * from `CostCard` where `Card` = '".._data.."';";
		local ret = sasql.query(token);
		if ret == 1 then
			sasql.free_result();
			sasql.store_result();
			local num = sasql.num_rows();
			if num <= 0 then
				char.TalkToCli(_talkerindex, -1, "您输入的卡号无效！", "红色");
				return;
			end
			sasql.fetch_row();
			local cardcf = other.atoi(sasql.data(2));--充值卡是否已使用
			if cardcf ~= 0 then
				char.TalkToCli(_talkerindex, -1, "您输入的卡密不存在或者已经使用过了！", "红色");
				return;
			end
			local cardtype = other.atoi(sasql.data(4));--卡密类型(1为金币，2为道具，3为宠物)
			local carddata = other.atoi(sasql.data(3));--卡密内容
			local cardstart = false;
			if cardtype == 1 then--金币卡
				token = "update `CSA_Login` set `VipPoint`=`VipPoint`+"..carddata.." where `Name`='"..char.getChar(_talkerindex, "账号").."'";
				sasql.query(token);
				char.TalkToCli(_talkerindex, -1, "充值成功！获得 "..carddata.." 金币", 4);
				cardstart = true;
				--额外赠送积分
				if CardGiveJf[1] == true then
					local givejf = math.floor(carddata / 100) * CardGiveJf[2];
					char.setInt(_talkerindex, "积分",char.getInt(_talkerindex, "积分")+givejf);
					if givejf > 0 then
						char.TalkToCli(_talkerindex, -1, "当前充值有赠送积分活动，赠送 "..givejf.." 积分！", 4);
					end
				end
				--额外赠送积分结束
			elseif cardtype == 2 then--道具卡
				if char.GetEmptyItemNum(_talkerindex) < 1 then
					char.TalkToCli(_talkerindex, -1, "抱歉！您的道具栏空位不足1个！", 6);
					cardstart = false;
					return;
				end
				char.Additem(_talkerindex, carddata);
				cardstart = true;
			elseif cardtype == 3 then--宠物卡
				if char.GetEmptyPetNum(_talkerindex) < 1 then
					char.TalkToCli(_talkerindex, -1, "抱歉！您的宠物栏空位不足1个！", 6);
					cardstart = false;
					return;
				end
				char.AddPet( _talkerindex, carddata,1);
				cardstart = true;
				
			end
			if cardstart == true then--使用成功时更新卡密信息
				token = "update `CostCard` set `CardCf`=1,`CardAccount`='"..char.getChar(_talkerindex, "账号").."',`CardName`='"..char.getChar(_talkerindex, "名字").."',`CardTime`="..os.time().." where `Card`='".._data.."'";
				sasql.query(token);
			end
		end
	end
end

function Create(_name, _metamo, _floor, _x, _y, _dir)
	npcindex = npc.CreateNpc(_name, _metamo, _floor, _x, _y, _dir);
	char.setFunctionPointer(npcindex, "对话事件", "Talked", "");
	char.setFunctionPointer(npcindex, "窗口事件", "WindowTalked", "");
end

function mydata()
	CardGiveJf = {true,100};
	npcdata = {"卡密充值员",101156,777,49,35,4};
end

function reloadnpc()
	print("重读NPC信息",char.getChar(npcindex, "名字"));
	mydata();
	char.setChar(npcindex, "名字",npcdata[1]);
	char.setInt(npcindex, "图像号", npcdata[2]);
	char.setInt(npcindex, "原图像号", npcdata[2]);
	char.WarpToSpecificPoint(npcindex, npcdata[3], npcdata[4], npcdata[5]);
	char.setInt(npcindex, "方向", npcdata[6]);
	char.ToAroundChar(npcindex);
end

function main()
	mydata()
	Create(npcdata[1], npcdata[2], npcdata[3], npcdata[4], npcdata[5], npcdata[6]);
	magic.addLUAListFunction("重读充值员", "reloadnpc", "", 1, "[重读远程组队]");
end


