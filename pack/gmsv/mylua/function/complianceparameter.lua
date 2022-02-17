function getIntPart(x)
    if x <= 0 then
       return math.ceil(x);
    end
    if math.ceil(x) == x then
       return math.ceil(x);
    else
       return math.ceil(x) - 1;
    end
end

--发送对像更新包时进行检测修复用
function FreeComplianceParameter( charaindex )
	if char.getInt(charaindex,"类型") == 1 then
		
	end
end

function data()
	
end

function main()
	data()
end
