package main

func ParseNamedArguments(args []string) map[string][]string {
	var argMap = make(map[string][]string, 0)
	for i := range args {
		var arg = args[i]
		if arg[0] == '-' && arg[1] == '-' {
			var equalIndex = 0
			var lPtr = 2
			for ; lPtr < len(arg); lPtr++ {
				if arg[lPtr] == '=' {
					equalIndex = lPtr
					break
				}
			}
			var key = arg[2:equalIndex]
			var value = arg[equalIndex+1:]
			argMap[key] = append(argMap[key], value)
		}
	}

	return argMap
}
