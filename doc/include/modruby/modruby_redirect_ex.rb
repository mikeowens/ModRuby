<%if @request.queries['type'] == 'external'%>
<meta http-equiv="refresh" content="5;url=http://www.ubuntu.com">
This is an external redirect. You will be sent to ubuntu.com in 5 seconds.
<%end%>

<%
if @request.queries['type'] == 'internal'
  @request.internal_redirect('/redirected.rhtml')
end
%>

<pre>
Click <a href="redirect.rhtml?type=internal">here</a> for internal redirect.
Click <a href="redirect.rhtml?type=external">here</a> for external redirect.
