--登录过程读取和验证
function FreeLoginCheck( fd,cdkey, passwd, ip, mac1 ,mac2,mac3 )
	--print(cdkey, passwd, ip, mac1 ,mac2,mac3 );
	if sasql.getLock(cdkey) == 1 then
		lssproto.ClientLogin(fd, "你的帐号存在非法操作，无法登录！");
		return 0;
	end
	--维护时限制GM登录
	local gmlevel = 0;
	local mycdkey = net.getCdkey(fd);
	local mylevel = char.GetGmLevel(mycdkey);
	if mylevel < gmlevel then
		lssproto.ClientLogin(fd, "线路维护中，无法登录...");
		return 0;
	end
	--单个线路最大登陆数限制
	local maxplayer = config.getFdnum() - 1;
	local macmaxnum = {0,0,0,1,0};--线路最大登录数(0为不限制)
	local serverid = config.getServernumber();
	local mynum = 0;
	for i = 1, maxplayer do
		if fd ~= i then
			if net.getUse(i) == 1 then
				if net.getIP(i) == net.getIP(fd) then--IP相同
					mynum = mynum + 1;
				end
			end
		end
	end
	if macmaxnum[serverid] > 0 then
		if mynum >= macmaxnum[serverid] then
			lssproto.ClientLogin(fd, "该线路最多只能登录 "..macmaxnum[serverid].." 个帐号，请勿过量登录！");
			return 0;
		end
	end
	
	--全部线路最多登陆数判断
	local token = "SELECT * FROM `CSA_MaxIp` WHERE `Account`='" .. mycdkey .. "'";
	local ret = sasql.query(token);
	if ret == 1 then
		sasql.free_result();
		sasql.store_result();
		local num = sasql.num_rows();
		if num > 0 then--如果该帐号有信息存在，先删除信息
			token = "DELETE FROM CSA_MaxIp WHERE Account='" ..  mycdkey.."';"
			sasql.query(token);
		end
	end
	token = "SELECT * FROM `CSA_MaxIp` WHERE `IP`='" .. ip .. "'";--从IP获取到数量
	ret = sasql.query(token);
	if ret == 1 then
		sasql.free_result();
		sasql.store_result();
		local num = sasql.num_rows();
		if num >= 20 then
			local accountstr = "";
			local hhf = 0;
			for i=1,num do
				sasql.fetch_row();
				hhf = hhf + 1;
				accountstr = accountstr .. sasql.data(1).."|";
				if hhf >= 10 then
					accountstr = accountstr .."\n";
					hhf = 0;
				end
			end
			lssproto.ClientLogin(fd, "您的IP登录账号数量已达上限，请不要过量进入游戏哦！\n目前已在线的帐号有：\n"..accountstr);
			return 0;
		end
	end
	--写入
	token = "INSERT INTO `CSA_MaxIp` SET `Account` = '" .. mycdkey.."',`IP`='".. ip .."',`MacId`='".. mac1 .."',`CpuId`='".. mac2 .."',ServerId="..serverid ..";";
	ret = sasql.query(token);
	--全部线路最多登陆数判断
	return 1;
end

function mydata()

end

function reload(charaindex, data)
	mydata();
end

function main()
	mydata();
	magic.addLUAListFunction("重读登录事件", "reload", "", 3, "[重读登录事件]");
end
