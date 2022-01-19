// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT
#pragma once

#include <azure/core/base64.hpp>

namespace Azure { namespace Security { namespace Attestation { namespace Test {

const std::string OpenEnclaveQuote(
      "AQAAAAIAAADoEQAAAAAAAAMAAgAAAAAABQAKAJOacjP3nEyplAoNs5V_Bgfl_L18zrEJejtqk6RDB0IzAAAAABERAwX_"
      "gAYAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAcAAAAAAAAABwAAAAAAAAApKh9LUZ5GYn6yR4o9mFFAVlPFtLCmkl3oQ4N"
      "NkhaFDgAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAASupfmg7QSxH4iarf5qHTdiE6Kalahc5zN65vf-"
      "zmYQwAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAEAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAKFQuRP5-c_ZhD2sxrnV2kl8JzNu0xWRlg-"
      "zBVhM3qP8AAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAADQQAACJx8e27oQ0pijs3lXQ9HfKWP9NMqVHQFL9SOjC_KGDcbv-I2fCafTHJ__"
      "AmNqVXy7XTXnzmLp1HhUCy1_9AORSAT"
      "qGZ1PtvBf4Q2NfNxqVkNrGJAjYuqMPStdg0MuM21nN-Qc9BWNycRMMsU7YfHSzmw7eGjBb_Ewfb3k6N4ZYRhERAwX_"
      "gAYAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABUAAAAAAAAABwAAAAAAAAA_sKzghp0uMPKOhtcMdmQDpU-"
      "7zWWO7ODhuUipFVkXQAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAjE9XddeWUD6WE393xoqCmgBWrI3tcBQLCBsJRJDFe_"
      "8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAUAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAH_"
      "mzVQFF8XbJCRGdNkA3SPx9ZUPgtx3874VyDYQnFRIAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAEUP2-pxe7LoyevtN5BdE4KKikxKK6-"
      "hwG0xCDmxmfLphcnrVskSbKmiKUfzkWUBehrF8gHCGNGIPX3QQDwmtZ4gAAABAgME"
      "BQYHCAkKCwwNDg8QERITFBUWFxgZGhscHR4fBQDMDQAALS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUVnVEND"
      "QkNhZ0F3SUJBZ0lVU3I5VmR"
      "iSnFWbzVyZzRadUpCRWo2SjNoak9jd0NnWUlLb1pJemowRUF3SXcKY1RFak1DRUdBMVVFQXd3YVNXNTBaV3dnVTBkWUl"
      "GQkRTeUJRY205alpYTnpiM0"
      "lnUTBFeEdqQVlCZ05WQkFvTQpFVWx1ZEdWc0lFTnZjbkJ2Y21GMGFXOXVNUlF3RWdZRFZRUUhEQXRUWVc1MFlTQkRiR0"
      "Z5WVRFTE1Ba0dBMVVFCkNBd"
      "0NRMEV4Q3pBSkJnTlZCQVlUQWxWVE1CNFhEVEl4TURNeE9UQTBOVEl3T0ZvWERUSTRNRE14T1RBME5USXcKT0Zvd2NER"
      "WlNQ0FHQTFVRUF3d1pTVzUw"
      "Wld3Z1UwZFlJRkJEU3lCRFpYSjBhV1pwWTJGMFpURWFNQmdHQTFVRQpDZ3dSU1c1MFpXd2dRMjl5Y0c5eVlYUnBiMjR4"
      "RkRBU0JnTlZCQWNNQzFOaGJ"
      "uUmhJRU5zWVhKaE1Rc3dDUVlEClZRUUlEQUpEUVRFTE1Ba0dBMVVFQmhNQ1ZWTXdXVEFUQmdjcWhrak9QUUlCQmdncWh"
      "rak9QUU1CQndOQ0FBUlQKVG"
      "RNNVhTMGFiRTA2ZUdVTVU3S1JOQXJlRGRtTWJHK25KVHlucDZXankyeXJ6NmlEa3h1R1F3WGZ1b25uUVBuZApjdHgwbH"
      "IyR3I0WjF1YXNsQjM2Vm80S"
      "UNtekNDQXBjd0h3WURWUjBqQkJnd0ZvQVUwT2lxMm5YWCtTNUpGNWc4CmV4UmwwTlh5V1Uwd1h3WURWUjBmQkZnd1ZqQ"
      "lVvRktnVUlaT2FIUjBjSE02"
      "THk5aGNHa3VkSEoxYzNSbFpITmwKY25acFkyVnpMbWx1ZEdWc0xtTnZiUzl6WjNndlkyVnlkR2xtYVdOaGRHbHZiaTky"
      "TWk5d1kydGpjbXcvWTJFOQp"
      "jSEp2WTJWemMyOXlNQjBHQTFVZERnUVdCQlRMejZNQ3VHcVZobFYrR2Q0ZGtacmx4YndCV2pBT0JnTlZIUThCCkFmOEV"
      "CQU1DQnNBd0RBWURWUjBUQV"
      "FIL0JBSXdBRENDQWRRR0NTcUdTSWI0VFFFTkFRU0NBY1V3Z2dIQk1CNEcKQ2lxR1NJYjRUUUVOQVFFRUVMOEhhRExXWW"
      "dVUFUzU3c3Tm1Ibkhrd2dnR"
      "mtCZ29xaGtpRytFMEJEUUVDTUlJQgpWREFRQmdzcWhraUcrRTBCRFFFQ0FRSUJFVEFRQmdzcWhraUcrRTBCRFFFQ0FnS"
      "UJFVEFRQmdzcWhraUcrRTBC"
      "CkRRRUNBd0lCQWpBUUJnc3Foa2lHK0UwQkRRRUNCQUlCQkRBUUJnc3Foa2lHK0UwQkRRRUNCUUlCQVRBUkJnc3EKaGtp"
      "RytFMEJEUUVDQmdJQ0FJQXd"
      "FQVlMS29aSWh2aE5BUTBCQWdjQ0FRWXdFQVlMS29aSWh2aE5BUTBCQWdnQwpBUUF3RUFZTEtvWklodmhOQVEwQkFna0N"
      "BUUF3RUFZTEtvWklodmhOQV"
      "EwQkFnb0NBUUF3RUFZTEtvWklodmhOCkFRMEJBZ3NDQVFBd0VBWUxLb1pJaHZoTkFRMEJBZ3dDQVFBd0VBWUxLb1pJaH"
      "ZoTkFRMEJBZzBDQVFBd0VBW"
      "UwKS29aSWh2aE5BUTBCQWc0Q0FRQXdFQVlMS29aSWh2aE5BUTBCQWc4Q0FRQXdFQVlMS29aSWh2aE5BUTBCQWhBQwpBU"
      "UF3RUFZTEtvWklodmhOQVEw"
      "QkFoRUNBUW93SHdZTEtvWklodmhOQVEwQkFoSUVFQkVSQWdRQmdBWUFBQUFBCkFBQUFBQUF3RUFZS0tvWklodmhOQVEw"
      "QkF3UUNBQUF3RkFZS0tvWkl"
      "odmhOQVEwQkJBUUdBSkJ1MVFBQU1BOEcKQ2lxR1NJYjRUUUVOQVFVS0FRQXdDZ1lJS29aSXpqMEVBd0lEU1FBd1JnSWh"
      "BSzZPMS9GNy80NFprcWhUN2"
      "FhNgp5QVh6QlltRWxUVHRvL25rVUd4N1BtUktBaUVBMXliSWt6SjVwcXR1L21jOW5DUWNwRUJOdk5KZFNIcW1jc04rCk"
      "V2dWJ3WlU9Ci0tLS0tRU5EI"
      "ENFUlRJRklDQVRFLS0tLS0KLS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUNsekNDQWo2Z0F3SUJBZ0lWQU5Eb"
      "3F0cDExL2t1U1JlWVBIc1Va"
      "ZERWOGxsTk1Bb0dDQ3FHU000OUJBTUMKTUdneEdqQVlCZ05WQkFNTUVVbHVkR1ZzSUZOSFdDQlNiMjkwSUVOQk1Sb3dH"
      "QVlEVlFRS0RCRkpiblJsYkN"
      "CRApiM0p3YjNKaGRHbHZiakVVTUJJR0ExVUVCd3dMVTJGdWRHRWdRMnhoY21FeEN6QUpCZ05WQkFnTUFrTkJNUXN3CkN"
      "RWURWUVFHRXdKVlV6QWVGdz"
      "B4T0RBMU1qRXhNRFExTURoYUZ3MHpNekExTWpFeE1EUTFNRGhhTUhFeEl6QWgKQmdOVkJBTU1Ha2x1ZEdWc0lGTkhXQ0"
      "JRUTBzZ1VISnZZMlZ6YzI5e"
      "UlFTkJNUm93R0FZRFZRUUtEQkZKYm5SbApiQ0JEYjNKd2IzSmhkR2x2YmpFVU1CSUdBMVVFQnd3TFUyRnVkR0VnUTJ4a"
      "GNtRXhDekFKQmdOVkJBZ01B"
      "a05CCk1Rc3dDUVlEVlFRR0V3SlZVekJaTUJNR0J5cUdTTTQ5QWdFR0NDcUdTTTQ5QXdFSEEwSUFCTDlxK05NcDJJT2cK"
      "dGRsMWJrL3VXWjUrVEdRbTh"
      "hQ2k4ejc4ZnMrZktDUTNkK3VEelhuVlRBVDJaaERDaWZ5SXVKd3ZOM3dOQnA5aQpIQlNTTUpNSnJCT2pnYnN3Z2Jnd0h"
      "3WURWUjBqQkJnd0ZvQVVJbV"
      "VNMWxxZE5JbnpnN1NWVXI5UUd6a25CcXd3ClVnWURWUjBmQkVzd1NUQkhvRVdnUTRaQmFIUjBjSE02THk5alpYSjBhV1"
      "pwWTJGMFpYTXVkSEoxYzNSb"
      "FpITmwKY25acFkyVnpMbWx1ZEdWc0xtTnZiUzlKYm5SbGJGTkhXRkp2YjNSRFFTNWpjbXd3SFFZRFZSME9CQllFRk5Eb"
      "wpxdHAxMS9rdVNSZVlQSHNV"
      "WmREVjhsbE5NQTRHQTFVZER3RUIvd1FFQXdJQkJqQVNCZ05WSFJNQkFmOEVDREFHCkFRSC9BZ0VBTUFvR0NDcUdTTTQ5"
      "QkFNQ0EwY0FNRVFDSUMvOWo"
      "rODRUK0h6dFZPL3NPUUJXSmJTZCsvMnVleEsKNCthQTBqY0ZCTGNwQWlBM2RoTXJGNWNENTJ0NkZxTXZBSXBqOFhkR21"
      "5MmJlZWxqTEpLK3B6cGNSQT"
      "09Ci0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0KLS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUNqakNDQWpTZ0"
      "F3SUJBZ0lVSW1VTTFscWROS"
      "W56ZzdTVlVyOVFHemtuQnF3d0NnWUlLb1pJemowRUF3SXcKYURFYU1CZ0dBMVVFQXd3UlNXNTBaV3dnVTBkWUlGSnZiM"
      "1FnUTBFeEdqQVlCZ05WQkFv"
      "TUVVbHVkR1ZzSUVOdgpjbkJ2Y21GMGFXOXVNUlF3RWdZRFZRUUhEQXRUWVc1MFlTQkRiR0Z5WVRFTE1Ba0dBMVVFQ0F3"
      "Q1EwRXhDekFKCkJnTlZCQVl"
      "UQWxWVE1CNFhEVEU0TURVeU1URXdOREV4TVZvWERUTXpNRFV5TVRFd05ERXhNRm93YURFYU1CZ0cKQTFVRUF3d1JTVzU"
      "wWld3Z1UwZFlJRkp2YjNRZ1"
      "EwRXhHakFZQmdOVkJBb01FVWx1ZEdWc0lFTnZjbkJ2Y21GMAphVzl1TVJRd0VnWURWUVFIREF0VFlXNTBZU0JEYkdGeV"
      "lURUxNQWtHQTFVRUNBd0NRM"
      "EV4Q3pBSkJnTlZCQVlUCkFsVlRNRmt3RXdZSEtvWkl6ajBDQVFZSUtvWkl6ajBEQVFjRFFnQUVDNm5Fd01ESVlaT2ova"
      "VBXc0N6YUVLaTcKMU9pT1NM"
      "UkZoV0dqYm5CVkpmVm5rWTR1M0lqa0RZWUwwTXhPNG1xc3lZamxCYWxUVll4RlAyc0pCSzV6bEtPQgp1ekNCdURBZkJn"
      "TlZIU01FR0RBV2dCUWlaUXp"
      "XV3AwMGlmT0R0SlZTdjFBYk9TY0dyREJTQmdOVkhSOEVTekJKCk1FZWdSYUJEaGtGb2RIUndjem92TDJObGNuUnBabWx"
      "qWVhSbGN5NTBjblZ6ZEdWa2"
      "MyVnlkbWxqWlhNdWFXNTAKWld3dVkyOXRMMGx1ZEdWc1UwZFlVbTl2ZEVOQkxtTnliREFkQmdOVkhRNEVGZ1FVSW1VTT"
      "FscWROSW56ZzdTVgpVcjlRR"
      "3prbkJxd3dEZ1lEVlIwUEFRSC9CQVFEQWdFR01CSUdBMVVkRXdFQi93UUlNQVlCQWY4Q0FRRXdDZ1lJCktvWkl6ajBFQ"
      "XdJRFNBQXdSUUlnUVFzLzA4"
      "cnljZFBhdUNGazhVUFFYQ01BbHNsb0JlN053YVFHVGNkcGEwRUMKSVFDVXQ4U0d2eEttanBjTS96MFdQOUR2bzhoMms1"
      "ZHUxaVdEZEJrQW4rMGlpQT0"
      "9Ci0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0KAA");

  const std::string OpenEnclaveNonDebugQuote(
      "AQAAAAIAAADoEQAAAAAAAAMAAgAAAAAABQAKAJOacjP3nEyplAoNs5V_BgfDYELtjmo1LWZEqF8xasaZAAAAABERAwX_"
      "gAYAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAUAAAAAAAAABwAAAAAAAAC3eSAmGL7LY2do5dkC8o1SQiJzX6-"
      "1OeqboHw_wXGhwgAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAALBpElSroIHE1xsKbdbjAKTcu6UtnfhXCC9QjQPENQaoAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABAAEAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAKFQuRP5-c_ZhD2sxrnV2kl8JzNu0xWRlg-"
      "zBVhM3qP8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADQQAA"
      "Ck3Zl7b4XERNDrAJfKbmzU0NSTIzY5rsFeyW_6deDzOyjXAnxA3b1WfBwrFrU9JKpfI9SDlpwms3YSQtJArmrD-8i_"
      "meLNk8sQSwSkR7TdIadTEAZdb7S-7qMt"
      "Yt3kWMfQ7H3g338R0kcF9wcy0jZmeBrRLDXyEUDcPI63Fwvp_BERAwX_"
      "gAYAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAABUAAAAAAA"
      "AABwAAAAAAAAA_sKzghp0uMPKOhtcMdmQDpU-"
      "7zWWO7ODhuUipFVkXQAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAjE9XddeWUD6WE393xoqCmgBW"
      "rI3tcBQLCBsJRJDFe_"
      "8AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAAAAAAAABAAUAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA"
      "AAAAAAAAAAAAAAAAAAmm22hOMEcuVw"
      "P_d08cVnQ2JF3uT3Q5as4s-"
      "D4yNshB0AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAMgCuq8kIluOwzRvIHV3J7G-"
      "62qz2UxkrKQ2aJHVWlzGwveCOd"
      "GIgpSJVM7GNcJW12FOveof_7m_"
      "nA78uk3D3C4gAAABAgMEBQYHCAkKCwwNDg8QERITFBUWFxgZGhscHR4fBQDMDQAALS0tLS1CRUdJTiBDRVJUSUZJQ0FU"
      "RS0t"
      "LS0tCk1JSUVnVENDQkNlZ0F3SUJBZ0lWQU9jMDJHVHYvUTRvOUhHS1Bndk8yeUpPMElWTk1Bb0dDQ3FHU000OUJBTUMK"
      "TUhFeEl6QWhCZ05WQkFNTUdrbHVkR1"
      "ZzSUZOSFdDQlFRMHNnVUhKdlkyVnpjMjl5SUVOQk1Sb3dHQVlEVlFRSwpEQkZKYm5SbGJDQkRiM0p3YjNKaGRHbHZiak"
      "VVTUJJR0ExVUVCd3dMVTJGdWRHRWdR"
      "MnhoY21FeEN6QUpCZ05WCkJBZ01Ba05CTVFzd0NRWURWUVFHRXdKVlV6QWVGdzB5TVRBME1Ea3hNalU1TWpoYUZ3MHlP"
      "REEwTURreE1qVTUKTWpoYU1IQXhJak"
      "FnQmdOVkJBTU1HVWx1ZEdWc0lGTkhXQ0JRUTBzZ1EyVnlkR2xtYVdOaGRHVXhHakFZQmdOVgpCQW9NRVVsdWRHVnNJRU"
      "52Y25CdmNtRjBhVzl1TVJRd0VnWURW"
      "UVFIREF0VFlXNTBZU0JEYkdGeVlURUxNQWtHCkExVUVDQXdDUTBFeEN6QUpCZ05WQkFZVEFsVlRNRmt3RXdZSEtvWkl6"
      "ajBDQVFZSUtvWkl6ajBEQVFjRFFnQU"
      "UKYzZ1TjVuZGlFY25mWUp1OFU1MUgzc3lzWm5kQ05JamlwRVdEZjJuaXp6VU9XdlZjdjIxWUUwakxMdmxpL2s3NAp2bj"
      "FtUzYvbkNYV3MxOFVZeE1PMFM2T0NB"
      "cHN3Z2dLWE1COEdBMVVkSXdRWU1CYUFGTkRvcXRwMTEva3VTUmVZClBIc1VaZERWOGxsTk1GOEdBMVVkSHdSWU1GWXdW"
      "S0JTb0ZDR1RtaDBkSEJ6T2k4dllYQn"
      "BMblJ5ZFhOMFpXUnoKWlhKMmFXTmxjeTVwYm5SbGJDNWpiMjB2YzJkNEwyTmxjblJwWm1sallYUnBiMjR2ZGpJdmNHTn"
      "JZM0pzUDJOaApQWEJ5YjJObGMzTnZj"
      "akFkQmdOVkhRNEVGZ1FVRzBZaHd5VEh2bFFUNFlQVU1lRGplMmFUUWNjd0RnWURWUjBQCkFRSC9CQVFEQWdiQU1Bd0dB"
      "MVVkRXdFQi93UUNNQUF3Z2dIVUJna3"
      "Foa2lHK0UwQkRRRUVnZ0hGTUlJQndUQWUKQmdvcWhraUcrRTBCRFFFQkJCRDBRZ2lrT3FkRWxVQnJYSWFPdCtVYU1JSU"
      "JaQVlLS29aSWh2aE5BUTBCQWpDQwpB"
      "VlF3RUFZTEtvWklodmhOQVEwQkFnRUNBUkV3RUFZTEtvWklodmhOQVEwQkFnSUNBUkV3RUFZTEtvWklodmhOCkFRMEJB"
      "Z01DQVFJd0VBWUxLb1pJaHZoTkFRME"
      "JBZ1FDQVFRd0VBWUxLb1pJaHZoTkFRMEJBZ1VDQVFFd0VRWUwKS29aSWh2aE5BUTBCQWdZQ0FnQ0FNQkFHQ3lxR1NJYj"
      "RUUUVOQVFJSEFnRUdNQkFHQ3lxR1NJ"
      "YjRUUUVOQVFJSQpBZ0VBTUJBR0N5cUdTSWI0VFFFTkFRSUpBZ0VBTUJBR0N5cUdTSWI0VFFFTkFRSUtBZ0VBTUJBR0N5"
      "cUdTSWI0ClRRRU5BUUlMQWdFQU1CQU"
      "dDeXFHU0liNFRRRU5BUUlNQWdFQU1CQUdDeXFHU0liNFRRRU5BUUlOQWdFQU1CQUcKQ3lxR1NJYjRUUUVOQVFJT0FnRU"
      "FNQkFHQ3lxR1NJYjRUUUVOQVFJUEFn"
      "RUFNQkFHQ3lxR1NJYjRUUUVOQVFJUQpBZ0VBTUJBR0N5cUdTSWI0VFFFTkFRSVJBZ0VLTUI4R0N5cUdTSWI0VFFFTkFR"
      "SVNCQkFSRVFJRUFZQUdBQUFBCkFBQU"
      "FBQUFBTUJBR0NpcUdTSWI0VFFFTkFRTUVBZ0FBTUJRR0NpcUdTSWI0VFFFTkFRUUVCZ0NRYnRVQUFEQVAKQmdvcWhraU"
      "crRTBCRFFFRkNnRUFNQW9HQ0NxR1NN"
      "NDlCQU1DQTBnQU1FVUNJUUNiNVBlMkd6ZkJTeWh2VDB4MgpXWEQ4MWlHYm8vMUs2cU9zRm1XNjg4R0FPZ0lnUzRiZjUr"
      "R0R6VExPUXpNMitESVJEc0VPelpUR1"
      "Jzc3pTOHFECm1lSVBKKzA9Ci0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0KLS0tLS1CRUdJTiBDRVJUSUZJQ0FURS0tLS"
      "0tCk1JSUNsekNDQWo2Z0F3SUJBZ0lW"
      "QU5Eb3F0cDExL2t1U1JlWVBIc1VaZERWOGxsTk1Bb0dDQ3FHU000OUJBTUMKTUdneEdqQVlCZ05WQkFNTUVVbHVkR1Zz"
      "SUZOSFdDQlNiMjkwSUVOQk1Sb3dHQV"
      "lEVlFRS0RCRkpiblJsYkNCRApiM0p3YjNKaGRHbHZiakVVTUJJR0ExVUVCd3dMVTJGdWRHRWdRMnhoY21FeEN6QUpCZ0"
      "5WQkFnTUFrTkJNUXN3CkNRWURWUVFH"
      "RXdKVlV6QWVGdzB4T0RBMU1qRXhNRFExTURoYUZ3MHpNekExTWpFeE1EUTFNRGhhTUhFeEl6QWgKQmdOVkJBTU1Ha2x1"
      "ZEdWc0lGTkhXQ0JRUTBzZ1VISnZZMl"
      "Z6YzI5eUlFTkJNUm93R0FZRFZRUUtEQkZKYm5SbApiQ0JEYjNKd2IzSmhkR2x2YmpFVU1CSUdBMVVFQnd3TFUyRnVkR0"
      "VnUTJ4aGNtRXhDekFKQmdOVkJBZ01B"
      "a05CCk1Rc3dDUVlEVlFRR0V3SlZVekJaTUJNR0J5cUdTTTQ5QWdFR0NDcUdTTTQ5QXdFSEEwSUFCTDlxK05NcDJJT2cK"
      "dGRsMWJrL3VXWjUrVEdRbThhQ2k4ej"
      "c4ZnMrZktDUTNkK3VEelhuVlRBVDJaaERDaWZ5SXVKd3ZOM3dOQnA5aQpIQlNTTUpNSnJCT2pnYnN3Z2Jnd0h3WURWUj"
      "BqQkJnd0ZvQVVJbVVNMWxxZE5Jbnpn"
      "N1NWVXI5UUd6a25CcXd3ClVnWURWUjBmQkVzd1NUQkhvRVdnUTRaQmFIUjBjSE02THk5alpYSjBhV1pwWTJGMFpYTXVk"
      "SEoxYzNSbFpITmwKY25acFkyVnpMbW"
      "x1ZEdWc0xtTnZiUzlKYm5SbGJGTkhXRkp2YjNSRFFTNWpjbXd3SFFZRFZSME9CQllFRk5EbwpxdHAxMS9rdVNSZVlQSH"
      "NVWmREVjhsbE5NQTRHQTFVZER3RUIv"
      "d1FFQXdJQkJqQVNCZ05WSFJNQkFmOEVDREFHCkFRSC9BZ0VBTUFvR0NDcUdTTTQ5QkFNQ0EwY0FNRVFDSUMvOWorODRU"
      "K0h6dFZPL3NPUUJXSmJTZCsvMnVleE"
      "sKNCthQTBqY0ZCTGNwQWlBM2RoTXJGNWNENTJ0NkZxTXZBSXBqOFhkR215MmJlZWxqTEpLK3B6cGNSQT09Ci0tLS0tRU"
      "5EIENFUlRJRklDQVRFLS0tLS0KLS0t"
      "LS1CRUdJTiBDRVJUSUZJQ0FURS0tLS0tCk1JSUNqakNDQWpTZ0F3SUJBZ0lVSW1VTTFscWROSW56ZzdTVlVyOVFHemtu"
      "QnF3d0NnWUlLb1pJemowRUF3SXcKYU"
      "RFYU1CZ0dBMVVFQXd3UlNXNTBaV3dnVTBkWUlGSnZiM1FnUTBFeEdqQVlCZ05WQkFvTUVVbHVkR1ZzSUVOdgpjbkJ2Y2"
      "1GMGFXOXVNUlF3RWdZRFZRUUhEQXRU"
      "WVc1MFlTQkRiR0Z5WVRFTE1Ba0dBMVVFQ0F3Q1EwRXhDekFKCkJnTlZCQVlUQWxWVE1CNFhEVEU0TURVeU1URXdOREV4"
      "TVZvWERUTXpNRFV5TVRFd05ERXhNRm"
      "93YURFYU1CZ0cKQTFVRUF3d1JTVzUwWld3Z1UwZFlJRkp2YjNRZ1EwRXhHakFZQmdOVkJBb01FVWx1ZEdWc0lFTnZjbk"
      "J2Y21GMAphVzl1TVJRd0VnWURWUVFI"
      "REF0VFlXNTBZU0JEYkdGeVlURUxNQWtHQTFVRUNBd0NRMEV4Q3pBSkJnTlZCQVlUCkFsVlRNRmt3RXdZSEtvWkl6ajBD"
      "QVFZSUtvWkl6ajBEQVFjRFFnQUVDNm"
      "5Fd01ESVlaT2ovaVBXc0N6YUVLaTcKMU9pT1NMUkZoV0dqYm5CVkpmVm5rWTR1M0lqa0RZWUwwTXhPNG1xc3lZamxCYW"
      "xUVll4RlAyc0pCSzV6bEtPQgp1ekNC"
      "dURBZkJnTlZIU01FR0RBV2dCUWlaUXpXV3AwMGlmT0R0SlZTdjFBYk9TY0dyREJTQmdOVkhSOEVTekJKCk1FZWdSYUJE"
      "aGtGb2RIUndjem92TDJObGNuUnBabW"
      "xqWVhSbGN5NTBjblZ6ZEdWa2MyVnlkbWxqWlhNdWFXNTAKWld3dVkyOXRMMGx1ZEdWc1UwZFlVbTl2ZEVOQkxtTnliRE"
      "FkQmdOVkhRNEVGZ1FVSW1VTTFscWRO"
      "SW56ZzdTVgpVcjlRR3prbkJxd3dEZ1lEVlIwUEFRSC9CQVFEQWdFR01CSUdBMVVkRXdFQi93UUlNQVlCQWY4Q0FRRXdD"
      "Z1lJCktvWkl6ajBFQXdJRFNBQXdSUU"
      "lnUVFzLzA4cnljZFBhdUNGazhVUFFYQ01BbHNsb0JlN053YVFHVGNkcGEwRUMKSVFDVXQ4U0d2eEttanBjTS96MFdQOU"
      "R2bzhoMms1ZHUxaVdEZEJrQW4rMGlp"
      "QT09Ci0tLS0tRU5EIENFUlRJRklDQVRFLS0tLS0KAA");

    std::string RuntimeJWKClaim = R"(
        {
            "jwk" : {
                "kty":"EC",
                "use":"sig",
                "crv":"P-256",
                "x":"18wHLeIgW9wVN6VD1Txgpqy2LszYkMf6J8njVAibvhM",
                "y":"cV4dS4UaLMgP_4fY4j8ir7cl1TXlFdAgcx55o7TkcSA"
            }
        }
        )";
  std::string EnclaveHeldData
      = "CiAgICAgICAgewogICAgICAgICAgICAiandrIiA6IHsKICAgICAgICAgICAgICAgICJrdHkiOiJFQyI"
      "sCiAgICAgICAgICAgICAgICAidXNlIjoic2lnIiwKICAgICAgICAgICAgICAgICJjcnYiOiJQLTI1Ni"
      "IsCiAgICAgICAgICAgICAgICAieCI6IjE4d0hMZUlnVzl3Vk42VkQxVHhncHF5MkxzellrTWY2Sjhua"
      "lZBaWJ2aE0iLAogICAgICAgICAgICAgICAgInkiOiJjVjRkUzRVYUxNZ1BfNGZZNGo4aXI3Y2wxVFhs"
      "RmRBZ2N4NTVvN1RrY1NBIgogICAgICAgICAgICB9CiAgICAgICAgfQogICAgICAgIAA";
  


 class AttestationCollateral {
public:
  static std::vector<uint8_t> OpenEnclaveReport()
  {
    return Azure::Core::_internal::Base64Url::Base64UrlDecode(OpenEnclaveQuote);
  }
  static std::vector<uint8_t> SgxQuote()
  {
    // An OpenEnclave report is a wrapper around an SGX quote - there are
    // 16 bytes of OpenEnclave header at the start of the report and what remains is
    // an SGX quote, so to convert from an OE report to an SGX quote simply strip
    // the first 16 bytes of the report.
    auto openEnclaveReport = OpenEnclaveReport();
    openEnclaveReport.erase(openEnclaveReport.begin(), openEnclaveReport.begin() + 16);
    return openEnclaveReport;
    //    return std::vector<uint8_t>(openEnclaveReport.begin() + 0x10, openEnclaveReport.end());
  }
  static std::vector<uint8_t> RuntimeData()
  {
    return Azure::Core::_internal::Base64Url::Base64UrlDecode(EnclaveHeldData);
    //    return std::vector<uint8_t>(RuntimeJWKClaim.begin(), RuntimeJWKClaim.end());
  }
};
}}}} // namespace Azure::Security::Attestation::Test
